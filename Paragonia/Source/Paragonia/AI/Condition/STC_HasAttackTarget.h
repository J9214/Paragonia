#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "STC_HasAttackTarget.generated.h"

USTRUCT()
struct FSTC_HasAttackTargetInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bMustHaveTarget = true;
};

USTRUCT(meta = (DisplayName = "Has Target", Category = "Condition|Combat"))
struct PARAGONIA_API FSTC_HasAttackTarget : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTC_HasAttackTargetInstanceData;

    FSTC_HasAttackTarget() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

    TStateTreeExternalDataHandle<ANpcBaseCharacter> NpcActorHandle;
};
