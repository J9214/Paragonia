// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/STT_ChaseTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

bool FSTT_ChaseTarget::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(NpcActorHandle);
	return true;
}

EStateTreeRunStatus FSTT_ChaseTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false ||
		IsValid(NPC->GetController()) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	AAIController* AI = Cast<AAIController>(NPC->GetController());
	if (IsValid(AI) &&
		IsValid(NPC->GetAttackTarget()))
	{
		AI->MoveToActor(NPC->GetAttackTarget(), NPC->GetAttackRange() - 5.0f, true, true, true, 0, true);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_ChaseTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false
		|| IsValid(NPC->GetAttackTarget()) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}