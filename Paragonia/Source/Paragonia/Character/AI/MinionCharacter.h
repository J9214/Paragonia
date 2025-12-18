// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "MinionCharacter.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class PARAGONIA_API AMinionCharacter : public ANpcBaseCharacter
{
	GENERATED_BODY()

public:
	AMinionCharacter();

public:
	UFUNCTION(BlueprintCallable, Category = "Minion|Path")
	void SetMovementSpline(USplineComponent* InSpline);

	UFUNCTION(BlueprintPure, Category = "Minion|Path")
	FVector GetTargetLocationOnSpline(float NextTargetDistance = 500.0f) const;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Minion|Path")
	TObjectPtr<USplineComponent> MovementSplineComponent;

};
