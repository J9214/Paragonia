#include "GA/Aurora/GA_SkillR_Aurora.h"
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

UGA_SkillR_Aurora::UGA_SkillR_Aurora()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_SkillR_Aurora::ActivateAbility(
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

	if (HasAuthority(&ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* HitResultTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Character.HitResult"));

		if (IsValid(HitResultTask))
		{
			HitResultTask->EventReceived.AddDynamic(this, &UGA_SkillR_Aurora::OnHitResultEvent);
			HitResultTask->ReadyForActivation();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_SkillR_Aurora::ActivateAbility - Failed to create HitResult Ability Task"));
		}
	}


	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("SkillR_Task"),
			AttackData.Montage,
			1.0f
		);

	if (IsValid(Task))
	{
		Task->OnCompleted.AddDynamic(this, &UGA_SkillR_Aurora::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_SkillR_Aurora::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_SkillR_Aurora::OnMontageCancelled);

		Task->ReadyForActivation();

		APGPlayerCharacterBase* PGCharacter = Cast<APGPlayerCharacterBase>(ActorInfo->AvatarActor.Get());
		if (IsValid(PGCharacter))
		{
			PGCharacter->SetInputLock(true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillR_Aurora::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_SkillR_Aurora::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	APGPlayerCharacterBase* PGCharacter = Cast<APGPlayerCharacterBase>(ActorInfo->AvatarActor.Get());
	if (IsValid(PGCharacter))
	{
		PGCharacter->SetInputLock(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SkillR_Aurora::OnHitResultEvent(const FGameplayEventData Payload)
{
	ApplyAttackDataEffects_OnHit(AttackData, Payload);
}

void UGA_SkillR_Aurora::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillR_Aurora::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillR_Aurora::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
