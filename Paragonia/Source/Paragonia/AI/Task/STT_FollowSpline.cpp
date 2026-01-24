// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/STT_FollowSpline.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "AIController.h"

bool FSTT_FollowSpline::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(MinionHandle);
	return true;
}

EStateTreeRunStatus FSTT_FollowSpline::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_FollowSpline::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AMinionCharacter* Minion = &Context.GetExternalData(MinionHandle);

	if (IsValid(Minion) == false ||
		IsValid(Minion->GetController()) == false ||
		IsValid(Minion->GetMovementSpline()) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	AAIController* AI = Cast<AAIController>(Minion->GetController());
	const USplineComponent* Spline = Minion->GetMovementSpline();

	FVector MyLocation = Minion->GetActorLocation();
	float CurrentDistance = Spline->GetDistanceAlongSplineAtLocation(MyLocation, ESplineCoordinateSpace::World);
	float TotalLength = Spline->GetSplineLength();

	if (CurrentDistance >= TotalLength - InstanceData.AcceptanceRadius)
	{
		AI->StopMovement();
		return EStateTreeRunStatus::Running;
	}

	float TargetDistance = CurrentDistance + InstanceData.LookAheadDistance;

	TargetDistance = FMath::Min(TargetDistance, TotalLength);

	FVector TargetLocation = Spline->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);

	AI->MoveToLocation(TargetLocation, -1.0f, false, true, true, true, nullptr, false);

	return EStateTreeRunStatus::Running;
}