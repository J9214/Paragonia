// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "STT_FindTarget.generated.h"

USTRUCT()
struct FSTT_FindTargetInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.1"))
    float ScanInterval = 1.0f;

    UPROPERTY(Transient)
    float CurrentTimer = 0.0f;
};

USTRUCT(meta = (DisplayName = "Find Target (C++)", Category = "Task|AI"))
struct PARAGONIA_API FSTT_FindTarget : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTT_FindTargetInstanceData;

    FSTT_FindTarget() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

    TStateTreeExternalDataHandle<ANpcBaseCharacter> NpcActorHandle;
};
