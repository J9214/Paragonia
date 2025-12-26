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

	AAIController* AI = Cast<AAIController>(NPC->GetController());
	AActor* Target = NPC->GetAttackTarget();

	if (IsValid(Target) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (Target->Implements<UPGTeamStatusInterface>() == true)
	{
		if (IPGTeamStatusInterface::Execute_GetIsDead(Target))
		{
			return EStateTreeRunStatus::Failed;
		}

		int32 MyTeamId = 255;
		if (NPC->Implements<UPGTeamStatusInterface>() == true)
		{
			MyTeamId = IPGTeamStatusInterface::Execute_GetTeamID(NPC);
		}

		if (MyTeamId != 255)
		{
			int32 TargetTeamId = IPGTeamStatusInterface::Execute_GetTeamID(Target);
			if (MyTeamId == TargetTeamId)
			{
				return EStateTreeRunStatus::Failed;
			}
		}
	}

	if (IsValid(AI) == true)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		AI->MoveToActor(Target, NPC->GetAttackRange() - InstanceData.DistanceCushion, true, true, true, 0, true);
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
		return EStateTreeRunStatus::Failed;
	}


	bool bShouldStop = false;

	if (Target->Implements<UPGTeamStatusInterface>() == true)
	{
		if (IPGTeamStatusInterface::Execute_GetIsDead(Target) == true)
		{
			bShouldStop = true;
		}
		else
		{
			int32 MyTeamId = 255;
			if (NPC->Implements<UPGTeamStatusInterface>() == true)
			{
				MyTeamId = IPGTeamStatusInterface::Execute_GetTeamID(NPC);
			}

			if (MyTeamId != 255)
			{
				int32 TargetTeamId = IPGTeamStatusInterface::Execute_GetTeamID(Target);
				if (MyTeamId == TargetTeamId)
				{
					bShouldStop = true;
				}
			}
		}
	}

	if (bShouldStop == true)
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