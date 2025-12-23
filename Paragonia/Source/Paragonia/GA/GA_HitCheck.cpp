#include "GA/GA_HitCheck.h"
#include "Character/PGPlayerCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "TargetActor/PGTargetActor.h"
#include "AbilitySystemComponent.h"
#include "Struct/AttackDataWrapper.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

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

	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_HitCheck::ActivateAbility - No TriggerEventData"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	RequestIdentifier = TriggerEventData->OptionalObject2;

	const UAttackDataWrapper* Wrapper = Cast<UAttackDataWrapper>(TriggerEventData->OptionalObject);
	if (!Wrapper)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitCheck Ability: Wrapper is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FAttackData AttackData = Wrapper->Data;

	UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
			this,
			TEXT("HitCheckTask"),
			EGameplayTargetingConfirmation::Instant,
			APGTargetActor::StaticClass()
		);

	Task->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	Task->Cancelled.AddDynamic(this, &ThisClass::OnTargetDataCancelled);

	AGameplayAbilityTargetActor* GenericActor = nullptr;
	if (Task->BeginSpawningActor(this, APGTargetActor::StaticClass(), GenericActor))
	{
		APGTargetActor* SpawnedActor = Cast<APGTargetActor>(GenericActor);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorLocation(ActorInfo->AvatarActor->GetActorLocation());
			SpawnedActor->SetAttackData(AttackData);
		}

		Task->FinishSpawningActor(this, GenericActor);
	}

	Task->ReadyForActivation();
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
	FGameplayEventData EventPayload;
	EventPayload.Instigator = GetAvatarActorFromActorInfo();
	EventPayload.TargetData = DataHandle;
	EventPayload.OptionalObject2 = RequestIdentifier.Get();

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetAvatarActorFromActorInfo(),
		FGameplayTag::RequestGameplayTag(FName("Event.Character.HitResult")),
		EventPayload
	);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_HitCheck::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
