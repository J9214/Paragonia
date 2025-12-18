#pragma once

#include "CoreMinimal.h"
#include "Struct/FAttackData.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GA_SpawnBullet_Sparrow.generated.h"

class APGTaskRelatedBullet;

UCLASS()
class PARAGONIA_API UGA_SpawnBullet_Sparrow : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SpawnBullet_Sparrow();

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

protected:
	FAttackData CurrentAttackData;
	TObjectPtr<APGTaskRelatedBullet> CurrentTargetActor;
	EGameplayTargetingConfirmation::Type CurrentConfimationType;
};
