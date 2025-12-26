#include "GA/PGGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitySpec.h"
#include "Struct/FAttackData.h"
#include "Struct/FAttackEffectEntry.h"
#include "GameplayTag/PGGameplayTags.h"
#include "PlayerState/PGPlayerState.h"
#include "GameFramework/Pawn.h"
#include "Interface/PGTeamStatusInterface.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

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
	const FGameplayAbilityTargetDataHandle SameTeamData = FilterTargetDataByTeamRule(TargetData, EPGTeamRule::SameTeamOnly);
	const FGameplayAbilityTargetDataHandle OtherTeamData = FilterTargetDataByTeamRule(TargetData, EPGTeamRule::OtherTeamOnly);

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

		const FGameplayAbilityTargetDataHandle* UseData = &TargetData;
		if (Entry.TeamRule == EPGTeamRule::SameTeamOnly)
		{
			UseData = &SameTeamData;
		}
		else if (Entry.TeamRule == EPGTeamRule::OtherTeamOnly)
		{
			UseData = &OtherTeamData;
		}

		if (UseData->Num() == 0)
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
			*UseData
		);
	}
}

int32 UPGGameplayAbilityBase::GetTeamIdFromActor(const AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return TEAM_NONE;
	}
	
	if (Actor->GetClass()->ImplementsInterface(UPGTeamStatusInterface::StaticClass()))
	{
		return IPGTeamStatusInterface::Execute_GetTeamID(Actor);
	}	

	return TEAM_NONE;
}

FGameplayAbilityTargetDataHandle UPGGameplayAbilityBase::FilterTargetDataByTeamRule(const FGameplayAbilityTargetDataHandle& InTargetData, EPGTeamRule Rule) const
{
	if (Rule == EPGTeamRule::Any)
	{
		return InTargetData;
	}

	const AActor* OwnerActor = GetOwningActorFromActorInfo();
	const int32 OwnerTeamID = GetTeamIdFromActor(OwnerActor);

	if (OwnerTeamID == TEAM_NONE)
	{
		return InTargetData;
	}

	FGameplayAbilityTargetDataHandle OutTargetData;

	for (int32 i = 0; i < InTargetData.Num(); ++i)
	{
		const FGameplayAbilityTargetData* Data = InTargetData.Get(i);
		if (!Data)
		{
			continue;
		}
		
		const FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(Data);
		AActor* TargetActor = HitData->HitResult.GetActor();
		if (!IsValid(TargetActor))
		{
			continue;
		}

		const int32 TargetTeamID = GetTeamIdFromActor(TargetActor);
		// GetTeamIdFromActor는 Pawn 기준으로 팀 ID를 반환하므로 Pawn이 아닌 Object/Minion 등은 TEAM_NONE 반환
		// - SameTeamOnly: skip
		// - OtherTeamOnly: pass
		bool bPass = false;
		if (TargetTeamID == INDEX_NONE)
		{
			bPass = (Rule == EPGTeamRule::OtherTeamOnly);
		}
		else
		{
			if (Rule == EPGTeamRule::SameTeamOnly)
			{
				bPass = (OwnerTeamID == TargetTeamID);
			}
			else if (Rule == EPGTeamRule::OtherTeamOnly)
			{
				bPass = (OwnerTeamID != TargetTeamID);
			}
		}

		if (bPass)
		{
			OutTargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitData->HitResult));
		}
	}

	return OutTargetData;
}
