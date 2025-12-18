#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct/FAttackData.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "PGTaskRelatedBullet.generated.h"

class UAbilityTask_WaitTargetData;
class UGameplayAbility;

UCLASS()
class PARAGONIA_API APGTaskRelatedBullet : public AActor
{
	GENERATED_BODY()
	
public:
	APGTaskRelatedBullet();

	virtual void InitBullet(const FAttackData& InAttackData);

protected:
	FAttackData AttackData;
};
