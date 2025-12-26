#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Struct/FAttackData.h"
#include "PGRangedTargetActor.generated.h"

UCLASS()
class PARAGONIA_API APGRangedTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	APGRangedTargetActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	void SetAttackData(const FAttackData& InAttackData);

protected:
	bool PerformTrace(TArray<FHitResult>& OutHits) const;

private:
	bool SphereTrace(TArray<FHitResult>& OutHits) const;

	bool BoxTrace(TArray<FHitResult>& OutHits) const;
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAttackData AttackData;
};
