#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_Attack_Aurora.generated.h"

UCLASS()
class PARAGONIA_API UGA_Attack_Aurora : public UPGGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Attack_Aurora();

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

	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
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
	void OnComboWindowOpen(const FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowClose(const FGameplayEventData Payload);

	UFUNCTION()
	void OnStartNextCombo(const FGameplayEventData Payload);

	void PlayCurrentCombo();

	void SetupComboWindowTask();

	void SetupHitResultTask();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TArray<FAttackData> ComboAttackDatas;

private:
	int32 CurrentComboIndex = 0;

	bool bComboInputQueued = false;

	bool bCanAcceptNextInput = false;

	bool bIsComboTransition = false;
};
