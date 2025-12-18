// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/MinionCharacter.h"
#include "Components/SplineComponent.h"

AMinionCharacter::AMinionCharacter()
{
}

void AMinionCharacter::SetMovementSpline(USplineComponent* InSpline)
{
	if (IsValid(InSpline) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMinionCharacter::SetMovementSpline - Parameter Is Null!"));
		return;
	}

	if (HasAuthority())
	{
		MovementSplineComponent = InSpline;
	}
}

FVector AMinionCharacter::GetTargetLocationOnSpline(float NextTargetDistance) const
{
	if (IsValid(MovementSplineComponent) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMinionCharacter::GetTargetLocationOnSpline - MovementSplineComponent Is Null!"));
		return GetActorLocation();
	}

	float CurrentDistance = MovementSplineComponent->GetDistanceAlongSplineAtLocation(
		GetActorLocation(),
		ESplineCoordinateSpace::World
	);

	float TargetDistance = CurrentDistance + NextTargetDistance;
	float SplineLen = MovementSplineComponent->GetSplineLength();

	if (TargetDistance > SplineLen)
	{
		TargetDistance = SplineLen;
	}

	return MovementSplineComponent->GetLocationAtDistanceAlongSpline(
		TargetDistance,
		ESplineCoordinateSpace::World
	);
}
