#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct/FAttackData.h"
#include "PGTaskRelatedBullet.generated.h"

class UAbilityTask_WaitTargetData;
class UGameplayAbility;

UCLASS()
class PARAGONIA_API APGTaskRelatedBullet : public AActor
{
	GENERATED_BODY()
	
public:
	APGTaskRelatedBullet();

	virtual void InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData);

protected:
	TObjectPtr<UGameplayAbility> Ability;
	TObjectPtr<UAbilityTask_WaitTargetData> Task;
	FAttackData AttackData;
};
