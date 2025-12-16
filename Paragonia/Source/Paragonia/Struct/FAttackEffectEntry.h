#pragma once

#include "CoreMinimal.h"
#include "FAttackEffectEntry.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EPGEffectApplyTarget : uint8
{
    Owner,
    Target
};

USTRUCT(BlueprintType)
struct FAttackEffectEntry
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> EffectClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPGEffectApplyTarget ApplyTarget = EPGEffectApplyTarget::Target;
};
