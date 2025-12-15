#include "ExecCalc/ExecCalc_Damage.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "GameplayTag/PGGameplayTags.h"

struct FDamageCapture
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);

    FDamageCapture()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, AttackPower, Source, true);
    }
};

static const FDamageCapture& GetDamageCapture()
{
    static FDamageCapture DamageCapture;
    return DamageCapture;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(GetDamageCapture().AttackPowerDef);
}

void UExecCalc_Damage::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    FAggregatorEvaluateParameters EvaluationParams;
    float AttackPower = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        GetDamageCapture().AttackPowerDef,
        EvaluationParams,
        AttackPower
    );

	const float BaseDamage = Spec.GetSetByCallerMagnitude(TAG_Data_Damage_Base, false, 0.0f);
    const float Multiplier = Spec.GetSetByCallerMagnitude(TAG_Data_Damage_Multiplier, false, 0.0f);

	const float TotalDamage = FMath::Max(0.0f, BaseDamage + (AttackPower * Multiplier));

    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetDamagedAttribute(), EGameplayModOp::Additive, TotalDamage));
}
