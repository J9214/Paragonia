// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Character/AI/MinionCharacter.h"
#include "Components/SplineComponent.h"
#include "STT_CheckingSplineDistance.generated.h"
USTRUCT()
struct FSTT_CheckingSplineDistanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float CheckInterval = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float MaxDeviateDistance = 1000.0f;

    UPROPERTY(Transient)
    float CurrentTimer = 0.0f;
};

USTRUCT(meta = (DisplayName = "Checking Spline Distance", Category = "Task|AI"))
struct PARAGONIA_API FSTT_CheckingSplineDistance : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTT_CheckingSplineDistanceData;

    FSTT_CheckingSplineDistance() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

    TStateTreeExternalDataHandle<AMinionCharacter> MinionHandle;
};