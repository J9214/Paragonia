#pragma once

#include "CoreMinimal.h"
#include "Animation/PGAnimInstance.h"
#include "PGRangedAnimInstance.generated.h"

UCLASS()
class PARAGONIA_API UPGRangedAnimInstance : public UPGAnimInstance
{
	GENERATED_BODY()

public:
	void SetBulletClass(TSubclassOf<AActor> NewBulletClass);
	void SetTimeWaited(const float& InTimeWaited);

protected:
	virtual void AnimNotify_HitCheck() override;

protected:
	TSubclassOf<AActor> BulletClass;
	float TimeWaited;
};
