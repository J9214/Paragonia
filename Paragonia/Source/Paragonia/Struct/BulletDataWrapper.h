#pragma once

#include "CoreMinimal.h"
#include "Struct/AttackDataWrapper.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BulletDataWrapper.generated.h"

UCLASS()
class PARAGONIA_API UBulletDataWrapper : public UAttackDataWrapper
{
	GENERATED_BODY()

public:
	TSubclassOf<AActor> BulletClass;
	FTransform BulletSpawnTransform;
	float TimeWaited;
};
