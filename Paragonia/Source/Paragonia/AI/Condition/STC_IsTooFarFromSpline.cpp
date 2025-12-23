// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Condition/STC_IsTooFarFromSpline.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

bool FSTC_IsTooFarFromSpline::Link(FStateTreeLinker& Linker)
{
    Linker.LinkExternalData(MinionHandle);
    return true;
}

bool FSTC_IsTooFarFromSpline::TestCondition(FStateTreeExecutionContext& Context) const
{
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    AMinionCharacter* Minion = &Context.GetExternalData(MinionHandle);

    if (IsValid(Minion) == false
        || !IsValid(Minion->GetMovementSpline()))
    {
        return false;
    }

    FVector MyLocation = Minion->GetActorLocation();

    FVector ClosestSplineLocation = Minion->GetMovementSpline()->FindLocationClosestToWorldLocation(MyLocation, ESplineCoordinateSpace::World);

    float DistSq = FVector::DistSquared(MyLocation, ClosestSplineLocation);
    float MaxDistSq = FMath::Square(InstanceData.MaxDeviateDistance);

    return DistSq > MaxDistSq;
}