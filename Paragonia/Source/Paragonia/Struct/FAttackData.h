#pragma once

#include "CoreMinimal.h"
#include "Struct/FAttackEffectEntry.h"
#include "GameplayTagContainer.h"
#include "FAttackData.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EPGAttackShape : uint8
{
    Sphere,
    Capsule,
    Box
};

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPGAttackShape SweepShape = EPGAttackShape::Sphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAttackEffectEntry> DamageEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAttackEffectEntry> BuffEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAttackEffectEntry> DebuffEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag HitResultTag;
};
