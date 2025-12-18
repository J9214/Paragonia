#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "PGSelfBuffAbility.generated.h"

UCLASS()
class PARAGONIA_API UPGSelfBuffAbility : public UPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UPGSelfBuffAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UFUNCTION()
	virtual void OnFinishDelay();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FAttackData AttackData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float Duration;
	
};
