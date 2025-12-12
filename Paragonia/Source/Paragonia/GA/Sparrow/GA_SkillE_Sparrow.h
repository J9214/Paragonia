#pragma once

#include "CoreMinimal.h"
#include "GA/PGSpawnActorGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_SkillE_Sparrow.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PARAGONIA_API UGA_SkillE_Sparrow : public UPGSpawnActorGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_SkillE_Sparrow();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
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
	void OnInputPressed(float InTimeWaited);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;

private:
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
