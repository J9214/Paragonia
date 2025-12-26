#pragma once

#include "CoreMinimal.h"
#include "GA/PGSpawnActorGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GA_SkillR_LtBelica.generated.h"

UCLASS()
class PARAGONIA_API UGA_SkillR_LtBelica : public UPGSpawnActorGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_SkillR_LtBelica();

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

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;

	TObjectPtr<ACharacter> PlayerCharacter;
};
