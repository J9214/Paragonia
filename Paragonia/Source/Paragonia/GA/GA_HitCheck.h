#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "GA_HitCheck.generated.h"

class UGameplayEffect;

UCLASS()
class PARAGONIA_API UGA_HitCheck : public UPGGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_HitCheck();

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
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	UFUNCTION()
	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& DataHandle);

private:
	UPROPERTY()
	TWeakObjectPtr<const UObject> RequestIdentifier;
};
