#include "GA/GA_Attack_Aurora.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Struct/FAttackData.h"
#include "Animation/PGAnimInstance.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/PGPlayerCharacterBase.h"
#include "AbilitySystemComponent.h"

UGA_Attack_Aurora::UGA_Attack_Aurora()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	bReplicateInputDirectly = true;
}

void UGA_Attack_Aurora::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentComboIndex = 0;
	bComboInputQueued = false;
	bCanAcceptNextInput = false;
	bIsComboTransition = false;

	PlayCurrentCombo();
}

void UGA_Attack_Aurora::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (bCanAcceptNextInput)
	{
		bComboInputQueued = true;
	}
}

void UGA_Attack_Aurora::PlayCurrentCombo()
{
	if (!ComboAttackDatas.IsValidIndex(CurrentComboIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	const FAttackData& CurrentData = ComboAttackDatas[CurrentComboIndex];

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character))
	{
		if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
		{
			if (UPGAnimInstance* PGAnimInstance = Cast<UPGAnimInstance>(AnimInstance))
			{
				PGAnimInstance->SetCurrentAttackData(CurrentData);
			}
		}
	}

	if (!IsValid(CurrentData.Montage))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Aurora::PlayCurrentCombo - Montage is invalid"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	SetupHitResultTask();

	SetupComboWindowTask();

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("AttackTask"),
			CurrentData.Montage,
			1.0f
		);
	if (IsValid(Task))
	{
		Task->OnCompleted.AddDynamic(this, &UGA_Attack_Aurora::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_Attack_Aurora::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_Attack_Aurora::OnMontageCancelled);

		Task->ReadyForActivation();

		if (IsValid(Character))
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Aurora::PlayCurrentCombo - Failed to create Montage Task"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_Attack_Aurora::SetupHitResultTask()
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	UAbilityTask_WaitGameplayEvent* HitResultTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.HitResult")
		);
	if (IsValid(HitResultTask))
	{
		HitResultTask->EventReceived.AddDynamic(this, &UGA_Attack_Aurora::OnHitResultEvent);
		HitResultTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Aurora::SetupHitResultTask - Failed to create task"));
	}
}

void UGA_Attack_Aurora::SetupComboWindowTask()
{
	UAbilityTask_WaitGameplayEvent* OpenTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowOpened")
		);
	if (IsValid(OpenTask))
	{
		OpenTask->EventReceived.AddDynamic(this, &UGA_Attack_Aurora::OnComboWindowOpen);
		OpenTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* CloseTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowClosed")
		);
	if (IsValid(CloseTask))
	{
		CloseTask->EventReceived.AddDynamic(this, &UGA_Attack_Aurora::OnComboWindowClose);
		CloseTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* StartNextTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.StartNextCombo")
		);
	if (IsValid(StartNextTask))
	{
		StartNextTask->EventReceived.AddDynamic(this, &UGA_Attack_Aurora::OnStartNextCombo);
		StartNextTask->ReadyForActivation();
	}
}

void UGA_Attack_Aurora::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Attack_Aurora::OnHitResultEvent(const FGameplayEventData Payload)
{
	if (!ComboAttackDatas.IsValidIndex(CurrentComboIndex))
	{
		return;
	}

	const FAttackData& CurrentData = ComboAttackDatas[CurrentComboIndex];
	if (!IsValid(CurrentData.DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Aurora::OnHitResultEvent - DamageEffectClass is invalid"));
		return;
	}

	ApplyGameplayEffectToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		Payload.TargetData,
		CurrentData.DamageEffectClass,
		1.0f
	);
}

void UGA_Attack_Aurora::OnComboWindowOpen(const FGameplayEventData Payload)
{
	bCanAcceptNextInput = true;
}

void UGA_Attack_Aurora::OnComboWindowClose(const FGameplayEventData Payload)
{
	bCanAcceptNextInput = false;
}

void UGA_Attack_Aurora::OnStartNextCombo(const FGameplayEventData Payload)
{
	if (bComboInputQueued && ComboAttackDatas.IsValidIndex(CurrentComboIndex + 1))
	{
		bComboInputQueued = false;
		bCanAcceptNextInput = false;
		++CurrentComboIndex;

		bIsComboTransition = true;

		PlayCurrentCombo();
	}
}

void UGA_Attack_Aurora::OnMontageCompleted()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Attack_Aurora::OnMontageInterrupted()
{
	if (bIsComboTransition)
	{
		bIsComboTransition = false;
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Attack_Aurora::OnMontageCancelled()
{
	if (bIsComboTransition)
	{
		bIsComboTransition = false;
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(Character))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
