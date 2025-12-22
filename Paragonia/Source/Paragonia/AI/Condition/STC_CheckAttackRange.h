#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "STC_CheckAttackRange.generated.h"

USTRUCT()
struct FSTC_CheckAttackRangeInstanceData
{
    GENERATED_BODY()
};

USTRUCT(meta = (DisplayName = "Is In Attack Range", Category = "Condition|Combat"))
struct PARAGONIA_API FSTC_CheckAttackRange : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTC_CheckAttackRangeInstanceData;

    FSTC_CheckAttackRange() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;

    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

    TStateTreeExternalDataHandle<ANpcBaseCharacter> NpcActorHandle;
};
