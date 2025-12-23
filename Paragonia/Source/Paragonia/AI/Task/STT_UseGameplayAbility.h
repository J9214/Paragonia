// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h" // Handle 사용을 위해 필요
#include "STT_UseGameplayAbility.generated.h"

// 런타임 데이터 및 에디터 설정값
USTRUCT()
struct FSTT_UseGameplayAbilityInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Ability"))
    FGameplayTag AbilityTag;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bWaitForEnd = true;

    UPROPERTY(Transient)
    FGameplayAbilitySpecHandle ActiveAbilityHandle;
};

USTRUCT(meta = (DisplayName = "Use Gameplay Ability (Tag)", Category = "Task|Combat"))
struct PARAGONIA_API FSTT_UseGameplayAbility : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTT_UseGameplayAbilityInstanceData;

    FSTT_UseGameplayAbility() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

    TStateTreeExternalDataHandle<ANpcBaseCharacter> NpcActorHandle;
};