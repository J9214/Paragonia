#pragma once

#include "CoreMinimal.h"
#include "Bullet/PGTaskRelatedBullet.h"
#include "Struct/FAttackData.h"
#include "PGMultiBulletCreator.generated.h"

class UGameplayAbility;
class UAbilityTask_WaitTargetData;

UCLASS()
class PARAGONIA_API APGMultiBulletCreator : public APGTaskRelatedBullet
{
	GENERATED_BODY()
	
public:	
	APGMultiBulletCreator();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void CreateBullets();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Actor")
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Actor")
	int32 BulletCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Actor")
	float AngleSpacing;
};
