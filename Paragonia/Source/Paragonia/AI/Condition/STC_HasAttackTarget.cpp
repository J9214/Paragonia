// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Condition/STC_HasAttackTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

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

    bool bHasTarget = IsValid(NPC->GetAttackTarget());

    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    return bHasTarget == InstanceData.bMustHaveTarget;
}