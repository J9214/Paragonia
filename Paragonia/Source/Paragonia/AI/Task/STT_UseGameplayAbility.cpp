// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/STT_UseGameplayAbility.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

bool FSTT_UseGameplayAbility::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(NpcActorHandle);
	return true;
}

EStateTreeRunStatus FSTT_UseGameplayAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	UAbilitySystemComponent* ASC = NPC->GetAbilitySystemComponent();
	if (IsValid(ASC) == false) 
	{
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.AbilityTag.IsValid() == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	FGameplayAbilitySpec* FoundSpec = nullptr;

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (IsValid(Spec.Ability) == true && 
			Spec.Ability->AbilityTags.HasTag(InstanceData.AbilityTag) == true)
		{
			FoundSpec = &Spec;
			break;
		}
	}

	if (FoundSpec == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (ASC->TryActivateAbility(FoundSpec->Handle) == false)
	{
		return EStateTreeRunStatus::Running;
	}

	if (InstanceData.bWaitForEnd == false)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	InstanceData.ActiveAbilityHandle = FoundSpec->Handle;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_UseGameplayAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	UAbilitySystemComponent* ASC = NPC->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(InstanceData.ActiveAbilityHandle);

	if (Spec == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (Spec->IsActive() == true)
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Succeeded;
}