#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_SkillQ_Aurora.generated.h"

class UAbilityTask_ApplyRootMotionMoveToForce;

UCLASS()
class PARAGONIA_API UGA_SkillQ_Aurora : public UPGGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_SkillQ_Aurora();

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

	UFUNCTION()
	void OnDashFinished();

	UFUNCTION()
	void OnDashStartEvent(const FGameplayEventData Payload);

	UFUNCTION()
	void OnDashStopEvent(const FGameplayEventData Payload);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.3f;

private:
	UPROPERTY()
	UAbilityTask_ApplyRootMotionMoveToForce* DashTask;
};
