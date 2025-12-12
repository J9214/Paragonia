#include "GA/GA_SkillQ_Aurora.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Struct/FAttackData.h"
#include "Animation/PGAnimInstance.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/PGPlayerCharacterBase.h"
#include "AbilitySystemComponent.h"

UGA_SkillQ_Aurora::UGA_SkillQ_Aurora()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_SkillQ_Aurora::ActivateAbility(
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

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character))
	{
		if (UAnimInstance* AI = Character->GetMesh()->GetAnimInstance())
		{
			if (auto PGAnimInstance = Cast<UPGAnimInstance>(AI))
			{
				PGAnimInstance->SetCurrentAttackData(AttackData);
			}
		}
	}

	if (!IsValid(AttackData.Montage))
	{
		return;
	}

	UAbilityTask_WaitGameplayEvent* DashStartTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Aurora.DashStart")
		);

	if (IsValid(DashStartTask))
	{
		DashStartTask->EventReceived.AddDynamic(this, &UGA_SkillQ_Aurora::OnDashStartEvent);
		DashStartTask->ReadyForActivation();
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("AttackTask"),
			AttackData.Montage,
			1.0f
		);

	if (IsValid(Task))
	{
		Task->OnCompleted.AddDynamic(this, &UGA_SkillQ_Aurora::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_SkillQ_Aurora::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_SkillQ_Aurora::OnMontageCancelled);

		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillQ_Aurora::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UGA_SkillQ_Aurora::EndAbility(
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

		APlayerController* PC = Cast<APlayerController>(Character->GetController());
		if (IsValid(PC) && PC->IsLocalController())
		{
			PC->SetIgnoreMoveInput(false);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SkillQ_Aurora::OnHitResultEvent(const FGameplayEventData Payload)
{
	if (Payload.TargetData.Num() == 0)
	{
		return;
	}

	if (!IsValid(AttackData.DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillQ_Aurora::OnHitResultEvent - DamageEffectClass is invalid"));
		return;
	}

	ApplyGameplayEffectToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		Payload.TargetData,
		AttackData.DamageEffectClass,
		1.0f
	);
}

void UGA_SkillQ_Aurora::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillQ_Aurora::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillQ_Aurora::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillQ_Aurora::OnDashFinished()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Character))
	{
		return;
	}

	if (DashTask)
	{
		DashTask->EndTask();
		DashTask = nullptr;
	}

	Character->GetCharacterMovement()->StopMovementImmediately();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillQ_Aurora::OnDashStartEvent(const FGameplayEventData Payload)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Character))
	{
		return;
	}

	AController* Controller = Character->GetController();
	if (IsValid(Controller) && Controller->IsLocalController())
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->SetIgnoreMoveInput(true);
		}
	}

	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	const FVector Forward = Character->GetActorForwardVector();
	const FVector StartLocation = Character->GetActorLocation();
	const FVector TargetLocation = StartLocation + Forward * DashDistance;

	DashTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		FName("SkillQ_Dash"),
		TargetLocation,
		DashDuration,
		false,
		EMovementMode::MOVE_Walking,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector::ZeroVector,
		0.0f
	);

	if (IsValid(DashTask))
	{
		DashTask->OnTimedOut.AddDynamic(this, &UGA_SkillQ_Aurora::OnDashFinished);
		DashTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UGA_SkillQ_Aurora::OnDashFinished);
		DashTask->ReadyForActivation();
	}
}