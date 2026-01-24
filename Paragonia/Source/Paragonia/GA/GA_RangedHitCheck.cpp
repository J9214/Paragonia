#include "GA/GA_RangedHitCheck.h"

#include "Character/PGPlayerCharacterBase.h"
#include "TargetActor/PGRangedTargetActor.h"
#include "Struct/AttackDataWrapper.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_RangedHitCheck::UGA_RangedHitCheck()
{
}

void UGA_RangedHitCheck::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_RangedHitCheck::ActivateAbility - No Authority"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_RangedHitCheck::ActivateAbility - CommitAbility Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_RangedHitCheck::ActivateAbility - No TriggerEventData"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const UAttackDataWrapper* Wrapper = Cast<UAttackDataWrapper>(TriggerEventData->OptionalObject);
	if (!Wrapper)
	{
		UE_LOG(LogTemp, Warning, TEXT("RangedHitCheck Ability: Wrapper is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FAttackData AttackData = Wrapper->Data;
	CurrentHitResultTag = AttackData.HitResultTag;

	UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
			this,
			TEXT("HitCheckTask"),
			EGameplayTargetingConfirmation::Instant,
			APGRangedTargetActor::StaticClass()
		);

	Task->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	Task->Cancelled.AddDynamic(this, &ThisClass::OnTargetDataCancelled);
	Task->ReadyForActivation();

	AGameplayAbilityTargetActor* GenericActor = nullptr;
	if (Task->BeginSpawningActor(this, APGRangedTargetActor::StaticClass(), GenericActor))
	{
		APGRangedTargetActor* SpawnedActor = Cast<APGRangedTargetActor>(GenericActor);
		if (SpawnedActor)
		{
			SpawnedActor->SourceActor = Cast<AActor>(Wrapper->GetOuter());
			if (IsValid(SpawnedActor->SourceActor))
			{
				SpawnedActor->SetActorLocation(SpawnedActor->SourceActor->GetActorLocation());
				SpawnedActor->SetAttackData(AttackData);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UGA_RangedHitCheck::ActivateAbility - Invalid SpawnedActor->SourceActor"));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
		}

		Task->FinishSpawningActor(this, GenericActor);
	}
}

void UGA_RangedHitCheck::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_RangedHitCheck::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	FGameplayEventData EventPayload;
	EventPayload.Instigator = GetAvatarActorFromActorInfo();
	EventPayload.TargetData = DataHandle;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetAvatarActorFromActorInfo(),
		CurrentHitResultTag,
		EventPayload
	);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_RangedHitCheck::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

