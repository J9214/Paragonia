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
    bool bHasValidTarget = IsValid(Target);

    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    return bHasValidTarget == InstanceData.bMustHaveTarget;
}