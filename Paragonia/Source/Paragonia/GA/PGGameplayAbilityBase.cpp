#include "GA/PGGameplayAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitySpec.h"

#include "Struct/FAttackData.h"
#include "Struct/FAttackEffectEntry.h"

#include "GameplayTag/PGGameplayTags.h"

bool UPGGameplayAbilityBase::HasNetAuthority() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	return ActorInfo->IsNetAuthority();
}

void UPGGameplayAbilityBase::ApplyAttackDataEffects_OnHit(const FAttackData& InAttackData, const FGameplayEventData& Payload)
{
	if (!HasNetAuthority())
	{
		return;
	}

	if (Payload.TargetData.Num() == 0)
	{
		return;
	}

	ApplyEntriesToTargets(InAttackData.DamageEffects, Payload.TargetData, true, InAttackData);
	ApplyEntriesToTargets(InAttackData.BuffEffects, Payload.TargetData, false, InAttackData);
	ApplyEntriesToTargets(InAttackData.DebuffEffects, Payload.TargetData, false, InAttackData);
}

void UPGGameplayAbilityBase::ApplyAttackDataOwnerEffects_OnActivate(const FAttackData& InAttackData)
{
	if (!HasNetAuthority())
	{
		return;
	}

	ApplyEntriesToOwner(InAttackData.BuffEffects, false, InAttackData);
	ApplyEntriesToOwner(InAttackData.DebuffEffects, false, InAttackData);
}

void UPGGameplayAbilityBase::ApplyEntriesToOwner(
	const TArray<FAttackEffectEntry>& Entries,
	bool bInjectDamageParams,
	const FAttackData& InAttackData
)
{
	for (const FAttackEffectEntry& Entry : Entries)
	{
		if (!IsValid(Entry.EffectClass))
		{
			continue;
		}

		if (Entry.ApplyTarget != EPGEffectApplyTarget::Owner)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Entry.EffectClass, GetAbilityLevel());
		if (!SpecHandle.IsValid())
		{
			continue;
		}

		UObject* SourceObj = GetCurrentSourceObject();
		SpecHandle.Data->GetContext().AddSourceObject(SourceObj ? SourceObj : this);

		if (bInjectDamageParams)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Base, InAttackData.BaseDamage);
			SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Multiplier, InAttackData.DamageMultiplier);
		}

		ApplyGameplayEffectSpecToOwner(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			SpecHandle
		);
	}
}

void UPGGameplayAbilityBase::ApplyEntriesToTargets(
	const TArray<FAttackEffectEntry>& Entries,
	const FGameplayAbilityTargetDataHandle& TargetData,
	bool bInjectDamageParams,
	const FAttackData& InAttackData
)
{
	for (const FAttackEffectEntry& Entry : Entries)
	{
		if (!Entry.EffectClass)
		{
			continue;
		}

		if (Entry.ApplyTarget != EPGEffectApplyTarget::Target)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Entry.EffectClass, GetAbilityLevel());
		if (!SpecHandle.IsValid())
		{
			continue;
		}

		if (bInjectDamageParams)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Base, InAttackData.BaseDamage);
			SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Multiplier, InAttackData.DamageMultiplier);
		}

		ApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			SpecHandle,
			TargetData
		);
	}
}