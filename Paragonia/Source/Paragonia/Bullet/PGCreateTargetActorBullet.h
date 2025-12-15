#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct/FAttackData.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "PGCreateTargetActorBullet.generated.h"

class AGameplayAbilityTargetActor;
class UAbilityTask_WaitTargetData;
class UGameplayAbility;

UCLASS()
class PARAGONIA_API APGCreateTargetActorBullet : public AActor
{
	GENERATED_BODY()
	
public:
	APGCreateTargetActorBullet();

	virtual void PostInitializeComponents() override;

	void InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData);

protected:
	AGameplayAbilityTargetActor* CreateTargetActor();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectClass;

	TObjectPtr<UGameplayAbility> Ability;
	TObjectPtr<UAbilityTask_WaitTargetData> Task;
	FAttackData AttackData;
};
