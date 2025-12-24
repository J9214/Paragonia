#include "GA/LtBelica/GA_SkillR_LtBelica.h"

#include "Animation/PGRangedAnimInstance.h"
#include "Character/PGPlayerCharacterBase.h"
#include "Struct/FAttackData.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

UGA_SkillR_LtBelica::UGA_SkillR_LtBelica()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	bReplicateInputDirectly = true;
}

void UGA_SkillR_LtBelica::ActivateAbility(
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
			if (auto PGAnimInstance = Cast<UPGRangedAnimInstance>(AI))
			{
				PGAnimInstance->SetCurrentAttackData(AttackData);
				PGAnimInstance->SetBulletClass(SpawnActorClass);
				PGAnimInstance->SetBulletSpawnTransform(Character->GetMesh()->GetSocketTransform(FName(TEXT("MuzzleStationary2"))));
			}
		}

		PlayerCharacter = Character;

		FRotator ControlRot = PlayerCharacter->GetControlRotation();
		PlayerCharacter->SetActorRotation(FRotator(0.0f, ControlRot.Yaw, 0.0f));
		PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	if (!IsValid(AttackData.Montage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
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
		Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);

		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillR_LtBelica::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	UAbilityTask_WaitGameplayEvent* HitResultTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AttackData.HitResultTag);

	if (IsValid(HitResultTask))
	{
		HitResultTask->EventReceived.AddDynamic(this, &ThisClass::OnHitResultEvent);
		HitResultTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillR_LtBelica::ActivateAbility - Failed to create HitResult Ability Task"));
	}
}

void UGA_SkillR_LtBelica::OnHitResultEvent(const FGameplayEventData Payload)
{
	ApplyAttackDataEffects_OnHit(AttackData, Payload);
}

void UGA_SkillR_LtBelica::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillR_LtBelica::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_SkillR_LtBelica::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}


