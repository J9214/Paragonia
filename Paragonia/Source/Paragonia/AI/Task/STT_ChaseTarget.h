// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "AIController.h" 
#include "STT_ChaseTarget.generated.h"

USTRUCT()
struct FSTT_ChaseTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceCushion = 50.0f;
};

USTRUCT(meta = (DisplayName = "Chase Target", Category = "Task|AI"))
struct PARAGONIA_API FSTT_ChaseTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_ChaseTargetInstanceData;

	FSTT_ChaseTarget() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<ANpcBaseCharacter> NpcActorHandle;
};