// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Condition/STC_HasAttackTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Interface/PGTeamStatusInterface.h"

bool FSTC_HasAttackTarget::Link(FStateTreeLinker& Linker)
{
    Linker.LinkExternalData(NpcActorHandle);
    return true;
}

bool FSTC_HasAttackTarget::TestCondition(FStateTreeExecutionContext& Context) const
{
    ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

    if (IsValid(NPC) == false)
    {
        return false;
    }

	AActor* Target = NPC->GetAttackTarget();
	bool bHasValidTarget = false;

	if (IsValid(Target) == true)
	{
		bHasValidTarget = true;

		if (Target->Implements<UPGTeamStatusInterface>() == true)
		{
			if (IPGTeamStatusInterface::Execute_GetIsDead(Target))
			{
				bHasValidTarget = false;
			}
			else
			{
				int32 MyTeamId = 255;
				if (NPC->Implements<UPGTeamStatusInterface>())
				{
					MyTeamId = IPGTeamStatusInterface::Execute_GetTeamID(NPC);
				}

				if (MyTeamId != 255)
				{
					int32 TargetTeamId = IPGTeamStatusInterface::Execute_GetTeamID(Target);
					if (MyTeamId == TargetTeamId)
					{
						bHasValidTarget = false;
					}
				}
			}
		}
	}

    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    return bHasValidTarget == InstanceData.bMustHaveTarget;
}