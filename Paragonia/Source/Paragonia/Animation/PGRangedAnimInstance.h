#pragma once

#include "CoreMinimal.h"
#include "Animation/PGAnimInstance.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "PGRangedAnimInstance.generated.h"

UCLASS()
class PARAGONIA_API UPGRangedAnimInstance : public UPGAnimInstance
{
	GENERATED_BODY()

public:
	void SetBulletClass(TSubclassOf<AActor> NewBulletClass);
	void SetTimeWaited(const float& InTimeWaited);
	void SetBulletSpawnTransform(const FTransform& InTransform);

protected:
	UFUNCTION()
	void AnimNotify_SpawnBullet();

	UFUNCTION()
	void AnimNotify_MoveLockOn();

	UFUNCTION()
	void AnimNotify_MoveLockOff();

protected:
	TSubclassOf<AActor> BulletClass;
	float TimeWaited;
	FTransform BulletSpawnTransform;
};
