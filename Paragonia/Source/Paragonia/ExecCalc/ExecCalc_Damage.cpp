#include "ExecCalc/ExecCalc_Damage.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "GameplayTag/PGGameplayTags.h"

struct FDamageCapture
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);

    FDamageCapture()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, AttackPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, Defense, Target, true);
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
    RelevantAttributesToCapture.Add(GetDamageCapture().DefenseDef);
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

	float Defense = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        GetDamageCapture().DefenseDef,
        EvaluationParams,
        Defense
	);

	const float AbilityBaseDamage = Spec.GetSetByCallerMagnitude(TAG_Data_Damage_Base, false, 0.0f);
    const float AttackPowerMultiplier = Spec.GetSetByCallerMagnitude(TAG_Data_Damage_Multiplier, false, 0.0f);
	const float RawDamage = FMath::Max(0.0f, AbilityBaseDamage + (AttackPower * AttackPowerMultiplier));

	const float SafeDefense = FMath::Max(0.0f, Defense);
	const float DamageMultiplier = K / (K + SafeDefense);

	const float FinalDamage = RawDamage * DamageMultiplier;

    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetDamagedAttribute(), EGameplayModOp::Additive, FinalDamage));
}
