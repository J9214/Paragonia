// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_NpcAttackBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PARAGONIA_API UGA_NpcAttackBase : public UPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_NpcAttackBase();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	virtual void OnAttackEventReceived(FGameplayEventData Payload);

	FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromActor(AActor* TargetActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;

	// State Tree에서 지정한 actor 저장용
	UPROPERTY(BlueprintReadOnly, Category = "Runtime Cache")
	TWeakObjectPtr<const AActor> CachedTargetActor;
};
