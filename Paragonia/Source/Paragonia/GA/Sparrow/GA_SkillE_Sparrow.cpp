#include "GA/Sparrow/GA_SkillE_Sparrow.h"

#include "Animation/PGRangedAnimInstance.h"
#include "Character/PGPlayerCharacterBase.h"
#include "Struct/FAttackData.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystemComponent.h"

UGA_SkillE_Sparrow::UGA_SkillE_Sparrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_SkillE_Sparrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!IsValid(AttackData.Montage))
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("SkillETask"),
			AttackData.Montage,
			1.0f,
			FName("ChargeStart")
		);

	if (IsValid(Task))
	{
		Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);

		Task->ReadyForActivation();

		MontageTask = Task;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Sparrow::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	UAbilityTask_WaitInputPress* InputPressTask =
		UAbilityTask_WaitInputPress::WaitInputPress(this);

	InputPressTask->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
	InputPressTask->ReadyForActivation();
}

void UGA_SkillE_Sparrow::OnHitResultEvent(const FGameplayEventData Payload)
{
	if (Payload.TargetData.Num() == 0)
	{
		return;
	}

	if (!IsValid(AttackData.DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Sparrow::OnHitResultEvent - DamageEffectClass is invalid"));
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

void UGA_SkillE_Sparrow::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillE_Sparrow::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_SkillE_Sparrow::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_SkillE_Sparrow::OnInputPressed(float InTimeWaited)
{
	if (!IsValid(MontageTask))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Sparrow::OnInputPressed - MontageTask is invalid"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (IsValid(Character))
	{
		if (UAnimInstance* AI = Character->GetMesh()->GetAnimInstance())
		{
			if (auto PGAnimInstance = Cast<UPGRangedAnimInstance>(AI))
			{
				PGAnimInstance->SetCurrentAttackData(AttackData);
				PGAnimInstance->SetBulletClass(SpawnActorClass);
				PGAnimInstance->SetTimeWaited(InTimeWaited);
			}
		}
	}

	MontageTask->EndTask();

	MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("SkillETask"),
			AttackData.Montage,
			1.0f,
			FName("ChargeEnd")
		);

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);

	MontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* HitResultTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Character.HitResult"));

	if (IsValid(HitResultTask))
	{
		HitResultTask->EventReceived.AddDynamic(this, &ThisClass::OnHitResultEvent);
		HitResultTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Sparrow::ActivateAbility - Failed to create HitResult Ability Task"));
	}
}
