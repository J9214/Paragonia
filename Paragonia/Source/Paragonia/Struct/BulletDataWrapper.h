#pragma once

#include "CoreMinimal.h"
#include "Struct/AttackDataWrapper.h"
#include "BulletDataWrapper.generated.h"

UCLASS()
class PARAGONIA_API UBulletDataWrapper : public UAttackDataWrapper
{
	GENERATED_BODY()

public:
	TSubclassOf<AActor> BulletClass;
};
