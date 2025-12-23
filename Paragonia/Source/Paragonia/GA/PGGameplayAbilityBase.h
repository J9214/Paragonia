#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Struct/FAttackEffectEntry.h"
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
	Based on HitResult (TargetData):
	- DamageEffects: Target
	- Buff/Debuff: ApplyTarget == Target
	*/
	void ApplyAttackDataEffects_OnHit(const FAttackData& InAttackData, const FGameplayEventData& Payload);

	/*
	Ability Activate on Owner:
	- Buff/Debuff: ApplyTarget == Owner
	*/
	void ApplyAttackDataOwnerEffects_OnActivate(const FAttackData& InAttackData);

private:
	bool HasNetAuthority() const;

	void ApplyEntriesToOwner(const TArray<FAttackEffectEntry>& Entries, bool bInjectDamageParams, const FAttackData& InAttackData);

	void ApplyEntriesToTargets(const TArray<FAttackEffectEntry>& Entries, const FGameplayAbilityTargetDataHandle& TargetData, bool bInjectDamageParams, const FAttackData& InAttackData);

	int32 GetTeamIdFromActor(const AActor* Actor) const;

	FGameplayAbilityTargetDataHandle FilterTargetDataByTeamRule(
		const FGameplayAbilityTargetDataHandle& InTargetData,
		EPGTeamRule Rule
	) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EPGAbilityInputID InputID = EPGAbilityInputID::None;

private:
	const int32 TEAM_NONE = -1;
};
