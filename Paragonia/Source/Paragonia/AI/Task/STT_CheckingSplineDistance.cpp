// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/STT_CheckingSplineDistance.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool FSTT_CheckingSplineDistance::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(MinionHandle);
	return true;
}

EStateTreeRunStatus FSTT_CheckingSplineDistance::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.CurrentTimer = FMath::RandRange(0.0f, 0.5f);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_CheckingSplineDistance::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AMinionCharacter* Minion = &Context.GetExternalData(MinionHandle);

	if (IsValid(Minion) == false ||
		IsValid(Minion->GetAttackTarget()) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	AActor* Target = Minion->GetAttackTarget();
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);

	if (TargetASC)
	{
		if (TargetASC->HasMatchingGameplayTag(InstanceData.DeadTag))
		{
			Minion->SetAttackTarget(nullptr);
			return EStateTreeRunStatus::Failed;
		}
	}

	InstanceData.CurrentTimer -= DeltaTime;
	if (InstanceData.CurrentTimer > 0.0f)
	{
		return EStateTreeRunStatus::Running;
	}

	InstanceData.CurrentTimer = InstanceData.CheckInterval;

	if (const USplineComponent* Spline = Minion->GetMovementSpline())
	{
		FVector MyLoc = Minion->GetActorLocation();
		FVector ClosestLoc = Spline->FindLocationClosestToWorldLocation(MyLoc, ESplineCoordinateSpace::World);

		float DistSq = FVector::DistSquared(MyLoc, ClosestLoc);
		float MaxDistSq = FMath::Square(Minion->GetSightRange());

		if (DistSq > MaxDistSq)
		{
			Minion->SetAttackTarget(nullptr);

			return EStateTreeRunStatus::Failed;
		}
	}

	if (Minion->CanAttack() == true)
	{
		Context.SendEvent(FGameplayTag::RequestGameplayTag(FName("Event.Combat.CanAttack")));
	}
	else
	{
		Context.SendEvent(FGameplayTag::RequestGameplayTag(FName("Event.Combat.CanChase")));
	}

	return EStateTreeRunStatus::Running;
}