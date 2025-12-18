#pragma once

#include "CoreMinimal.h"
#include "Bullet/PGTaskRelatedBullet.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "PGCreateTargetActorBullet.generated.h"

class AGameplayAbilityTargetActor;

UCLASS()
class PARAGONIA_API APGCreateTargetActorBullet : public APGTaskRelatedBullet
{
	GENERATED_BODY()
	
public:
	APGCreateTargetActorBullet();

	virtual void PostInitializeComponents() override;

protected:
	AGameplayAbilityTargetActor* CreateTargetActor();
};
