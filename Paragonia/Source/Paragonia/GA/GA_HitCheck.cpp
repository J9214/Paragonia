#include "GA/GA_HitCheck.h"
#include "Character/PGPlayerCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "TargetActor/PGTargetActor.h"
#include "AbilitySystemComponent.h"

UGA_HitCheck::UGA_HitCheck()
{
}

void UGA_HitCheck::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_HitCheck::ActivateAbility - No Authority"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_HitCheck::ActivateAbility - CommitAbility Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
			this,
			TEXT("HitCheckTask"),
			EGameplayTargetingConfirmation::Instant,
			APGTargetActor::StaticClass()
		);

	Task->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	Task->ReadyForActivation();

	AGameplayAbilityTargetActor* GenericActor = nullptr;
	if (Task->BeginSpawningActor(this, APGTargetActor::StaticClass(), GenericActor))
	{
		APGTargetActor* SpawnedActor = Cast<APGTargetActor>(GenericActor);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorLocation(ActorInfo->AvatarActor->GetActorLocation());
		}

		Task->FinishSpawningActor(this, GenericActor);
	}
}

void UGA_HitCheck::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_HitCheck::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	TSet<APGPlayerCharacterBase*> UniqueTargets;
	for (int32 i = 0; i < DataHandle.Num(); ++i)
	{
		const FGameplayAbilityTargetData* Data = DataHandle.Get(i);

		if (const FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(Data))
		{
			APGPlayerCharacterBase* HitActor = Cast<APGPlayerCharacterBase>(HitData->HitResult.GetActor());
			if (IsValid(HitActor))
			{
				UniqueTargets.Add(HitActor);
			}
		}
	}

	for (APGPlayerCharacterBase* HitCharacter : UniqueTargets)
	{
		if (!IsValid(HitCharacter) || !IsValid(DamageEffectClass))
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = HitCharacter->GetAbilitySystemComponent();
		if (IsValid(TargetASC))
		{
			ApplyGameplayEffectToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				DataHandle,
				DamageEffectClass,
				1.0f
			);
		}
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_HitCheck::OnTargetDataCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
