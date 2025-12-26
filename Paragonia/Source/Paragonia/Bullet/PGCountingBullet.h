#pragma once

#include "CoreMinimal.h"
#include "Bullet/PGCreateTargetActorBullet.h"
#include "PGCountingBullet.generated.h"

class AGameplayAbilityTargetActor;

UCLASS()
class PARAGONIA_API APGCountingBullet : public APGCreateTargetActorBullet
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void SetSpawnTimer();

	UFUNCTION()
	void OnCreateTargetActor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnEffectCreate();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Interval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentCount;

	FTimerHandle SpawnActorTimer;
	TObjectPtr<AGameplayAbilityTargetActor> TargetActor;
};
