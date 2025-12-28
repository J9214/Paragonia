// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/Npc/GA_NpcAttackBase.h"
#include "GA_NpcAttackRange.generated.h"

class ANpcHomingProj;

UCLASS()
class PARAGONIA_API UGA_NpcAttackRange : public UGA_NpcAttackBase
{
	GENERATED_BODY()
public:
	UGA_NpcAttackRange();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void OnMontageCompleted() override;

	virtual void OnAttackEventReceived(FGameplayEventData Payload) override;

	UFUNCTION()
	void OnFireEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnTimeOut();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
	TSubclassOf<ANpcHomingProj> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
	float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
	FGameplayTag FireEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
	float MaxWaitTime;
};
