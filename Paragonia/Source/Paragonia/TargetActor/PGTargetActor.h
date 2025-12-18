#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Struct/FAttackData.h"
#include "PGTargetActor.generated.h"

struct FAttackData;

UCLASS()
class PARAGONIA_API APGTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	APGTargetActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	void SetAttackData(const FAttackData& InAttackData);

protected:
	bool PerformTrace(TArray<FHitResult>& OutHits) const;

private:
	bool SphereTrace(TArray<FHitResult>& OutHits) const;

	bool CapsuleTrace(TArray<FHitResult>& OutHits) const;

	bool BoxTrace(TArray<FHitResult>& OutHits) const;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAttackData AttackData;
};
