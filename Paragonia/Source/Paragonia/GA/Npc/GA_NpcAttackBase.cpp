// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Npc/GA_NpcAttackBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

UGA_NpcAttackBase::UGA_NpcAttackBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 서버에서만 돌게 하도록 (AI)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_NpcAttackBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(AttackData.Montage) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = 
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackData.Montage
	);

	if (IsValid(MontageTask))
	{
		MontageTask->OnCompleted.AddDynamic(this, &UGA_NpcAttackBase::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_NpcAttackBase::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_NpcAttackBase::OnMontageCancelled);
		MontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_NpcAttackBase::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitEventTask = 
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(FName("Event.Npc.HitResult")),
		nullptr, false, false
	);

	if (IsValid(WaitEventTask))
	{
		WaitEventTask->EventReceived.AddDynamic(this, &UGA_NpcAttackBase::OnAttackEventReceived);
		WaitEventTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_NpcAttackBase::ActivateAbility - Failed to create HitResult Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_NpcAttackBase::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_NpcAttackBase::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_NpcAttackBase::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_NpcAttackBase::OnAttackEventReceived(FGameplayEventData Payload)
{
	// Melee,Range 에서 override
}

FGameplayAbilityTargetDataHandle UGA_NpcAttackBase::MakeTargetDataHandleFromActor(AActor* TargetActor)
{
	if(TargetActor == nullptr)
		return FGameplayAbilityTargetDataHandle();

	FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit();
	NewData->HitResult.HitObjectHandle = FActorInstanceHandle(TargetActor);
	NewData->HitResult.bBlockingHit = true;

	FGameplayAbilityTargetDataHandle Handle;
	Handle.Add(NewData);

	return Handle;
}