#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "Character/AI/MinionCharacter.h"
#include "Components/SplineComponent.h"
#include "STC_IsTooFarFromSpline.generated.h"

USTRUCT()
struct FSTC_IsTooFarFromSplineInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0"))
    float MaxDeviateDistance = 1000.0f;
};

USTRUCT(meta = (DisplayName = "Is Too Far From Spline", Category = "Condition|Movement"))
struct PARAGONIA_API FSTC_IsTooFarFromSpline : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTC_IsTooFarFromSplineInstanceData;

    FSTC_IsTooFarFromSpline() = default;

    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

    TStateTreeExternalDataHandle<AMinionCharacter> MinionHandle;
};