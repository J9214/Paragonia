#include "GA/Greystone/GA_Attack_Greystone.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Struct/FAttackData.h"
#include "Animation/PGAnimInstance.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/PGPlayerCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/PGGameplayTags.h"

UGA_Attack_Greystone::UGA_Attack_Greystone()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	bReplicateInputDirectly = true;
}

void UGA_Attack_Greystone::ActivateAbility(
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

void UGA_Attack_Greystone::InputPressed(
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

void UGA_Attack_Greystone::PlayCurrentCombo()
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
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Greystone::PlayCurrentCombo - Montage is invalid"));
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
		Task->OnCompleted.AddDynamic(this, &UGA_Attack_Greystone::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_Attack_Greystone::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_Attack_Greystone::OnMontageCancelled);

		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Greystone::PlayCurrentCombo - Failed to create Montage Task"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_Attack_Greystone::SetupHitResultTask()
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	if (IsValid(HitResultTask))
	{
		HitResultTask->EndTask();
		HitResultTask = nullptr;
	}

	HitResultTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Character.HitResult"),
		nullptr,
		true,
		true
	);
	if (IsValid(HitResultTask))
	{
		HitResultTask->EventReceived.AddDynamic(this, &UGA_Attack_Greystone::OnHitResultEvent);
		HitResultTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Attack_Greystone::SetupHitResultTask - Failed to create task"));
	}
}

void UGA_Attack_Greystone::SetupComboWindowTask()
{
	UAbilityTask_WaitGameplayEvent* OpenTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowOpened")
		);
	if (IsValid(OpenTask))
	{
		OpenTask->EventReceived.AddDynamic(this, &UGA_Attack_Greystone::OnComboWindowOpen);
		OpenTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* CloseTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowClosed")
		);
	if (IsValid(CloseTask))
	{
		CloseTask->EventReceived.AddDynamic(this, &UGA_Attack_Greystone::OnComboWindowClose);
		CloseTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* StartNextTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Character.StartNextCombo")
		);
	if (IsValid(StartNextTask))
	{
		StartNextTask->EventReceived.AddDynamic(this, &UGA_Attack_Greystone::OnStartNextCombo);
		StartNextTask->ReadyForActivation();
	}
}

void UGA_Attack_Greystone::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Attack_Greystone::OnHitResultEvent(const FGameplayEventData Payload)
{
	const FAttackData& CurrentData = ComboAttackDatas[CurrentComboIndex];
	ApplyAttackDataEffects_OnHit(CurrentData, Payload);
}

void UGA_Attack_Greystone::OnComboWindowOpen(const FGameplayEventData Payload)
{
	bCanAcceptNextInput = true;
}

void UGA_Attack_Greystone::OnComboWindowClose(const FGameplayEventData Payload)
{
	bCanAcceptNextInput = false;
}

void UGA_Attack_Greystone::OnStartNextCombo(const FGameplayEventData Payload)
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

void UGA_Attack_Greystone::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Attack_Greystone::OnMontageInterrupted()
{
	if (bIsComboTransition)
	{
		bIsComboTransition = false;
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Attack_Greystone::OnMontageCancelled()
{
	if (bIsComboTransition)
	{
		bIsComboTransition = false;
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
