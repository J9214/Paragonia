// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/ObjectAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UObjectAttributeSet::UObjectAttributeSet()
{
	InitMaxHealth(200.f);
	InitHealth(200.f);
	InitDefense(0.0f);
	InitAttackPower(20.f);
	InitMoveSpeed(0.f);
}

void UObjectAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max<float>(1.0f, NewValue);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetDefenseAttribute())
	{
		NewValue = FMath::Max<float>(0.0f, NewValue);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max<float>(0.0f, NewValue);
	}
	else if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Max<float>(0.0f, NewValue);
	}
}

void UObjectAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(OldValue, NewValue);
	}
	else if (Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(OldValue, NewValue);
	}
}

void UObjectAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	if (Attribute == GetDamagedAttribute())
	{
		float LocalDamage = GetDamaged();
		SetDamaged(0.f);

		if (LocalDamage > 0.f)
		{
			float OldHealth = GetHealth();
			float NewHealth = FMath::Clamp(OldHealth - LocalDamage, 0.f, GetMaxHealth());
			SetHealth(NewHealth);
		}
	}
}

void UObjectAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UObjectAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObjectAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObjectAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObjectAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObjectAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UObjectAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObjectAttributeSet, MaxHealth, OldMaxHealth);
}

void UObjectAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObjectAttributeSet, Health, OldHealth);
}

void UObjectAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObjectAttributeSet, Defense, OldDefense);
}

void UObjectAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObjectAttributeSet, AttackPower, OldAttackPower);
}

void UObjectAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObjectAttributeSet, MoveSpeed, OldMoveSpeed);
}

