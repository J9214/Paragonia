#include "STC_CheckAttackRange.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

bool FSTC_CheckAttackRange::Link(FStateTreeLinker& Linker)
{
    Linker.LinkExternalData(NpcActorHandle);
    return true;
}

bool FSTC_CheckAttackRange::TestCondition(FStateTreeExecutionContext& Context) const
{
    ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);
    if (IsValid(NPC) == false ||
        IsValid(NPC->GetAttackTarget()) == false)
    {
        return false;
    }

    float DistSq = FVector::DistSquared(NPC->GetActorLocation(), 
        NPC->GetAttackTarget()->GetActorLocation());

    float MyRadius = NPC->GetSimpleCollisionRadius();
    float TargetRadius = NPC->GetAttackTarget()->GetSimpleCollisionRadius();

    float RealAttackRange = NPC->GetAttackRange() + MyRadius + TargetRadius;
    float RealAttackRangeSq = FMath::Square(RealAttackRange);

    return DistSq <= RealAttackRangeSq;
}