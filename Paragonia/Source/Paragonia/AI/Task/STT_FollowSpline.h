// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Character/AI/MinionCharacter.h"
#include "Components/SplineComponent.h"
#include "STT_FollowSpline.generated.h"

USTRUCT()
struct FSTT_FollowSplineInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "100.0"))
    float LookAheadDistance = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float AcceptanceRadius = 100.0f;
};

USTRUCT(meta = (DisplayName = "Follow Spline Path", Category = "Task|Movement"))
struct PARAGONIA_API FSTT_FollowSpline : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTT_FollowSplineInstanceData;

    FSTT_FollowSpline() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

    TStateTreeExternalDataHandle<AMinionCharacter> MinionHandle;
};