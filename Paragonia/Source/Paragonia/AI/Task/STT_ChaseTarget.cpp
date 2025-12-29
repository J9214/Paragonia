// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/STT_ChaseTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Interface/PGTeamStatusInterface.h"

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

	AActor* Target = NPC->GetAttackTarget();

	if (IsValid(Target) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	AAIController* AI = Cast<AAIController>(NPC->GetController());

	if (IsValid(AI) == true)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		float AcceptanceRadius = NPC->GetAttackRange() - InstanceData.DistanceCushion;
		AcceptanceRadius = FMath::Max(0.0f, AcceptanceRadius);
		AI->MoveToActor(Target, AcceptanceRadius, true, true, true, 0, true);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_ChaseTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false ||
		IsValid(NPC->GetController()) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	AActor* Target = NPC->GetAttackTarget();
	if (IsValid(Target) == false)
	{
		if (AAIController* AI = Cast<AAIController>(NPC->GetController()))
		{
			AI->StopMovement();
		}

		NPC->SetAttackTarget(nullptr);
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}