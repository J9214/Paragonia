#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PGGameplayAbilityBase.generated.h"

struct FAttackData;
struct FAttackEffectEntry;
struct FGameplayEventData;
struct FGameplayAbilityTargetDataHandle;

UENUM(BlueprintType)
enum class EPGAbilityInputID : uint8
{
	None,
	HitCheck,
	Jump,
	Attack,
	SkillQ,
	SkillE,
	SkillR
};

UCLASS()
class PARAGONIA_API UPGGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/* 
	HitResult(타겟 데이터) 기반으로 적용:
	- DamageEffects: Target
	- Buff/Debuff: ApplyTarget==Target 인 것만
	*/
	void ApplyAttackDataEffects_OnHit(const FAttackData& InAttackData, const FGameplayEventData& Payload);

	/*
	Ability Activate 시점(Owner) 적용:
	- Buff/Debuff: ApplyTarget==Owner 인 것만
	*/
	void ApplyAttackDataOwnerEffects_OnActivate(const FAttackData& InAttackData);

private:
	bool HasNetAuthority() const;

	void ApplyEntriesToOwner(const TArray<FAttackEffectEntry>& Entries, bool bInjectDamageParams, const FAttackData& InAttackData);

	void ApplyEntriesToTargets(const TArray<FAttackEffectEntry>& Entries, const FGameplayAbilityTargetDataHandle& TargetData, bool bInjectDamageParams, const FAttackData& InAttackData);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EPGAbilityInputID InputID = EPGAbilityInputID::None;


};
