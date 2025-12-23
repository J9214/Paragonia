#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_SkillE_Greystone.generated.h"

class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class PARAGONIA_API UGA_SkillE_Greystone : public UPGGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_SkillE_Greystone();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UFUNCTION()
	void OnHitResultEvent(const FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	void StartAura();
	void StopAura();

	void TickHitCheck();
	void OnAuraDurationFinished();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura")
	float AuraLifeTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura")
	float AuraPeriod = 1.0f;

private:
	FTimerHandle HitCheckTimerHandle;
	FTimerHandle AuraEndTimerHandle;
};
