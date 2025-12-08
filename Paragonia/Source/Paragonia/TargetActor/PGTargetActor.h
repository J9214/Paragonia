#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "PGTargetActor.generated.h"

UCLASS()
class PARAGONIA_API APGTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	APGTargetActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

private:
	bool GetSphereTraceHitResult(AActor* InSourceActor, TArray<FHitResult>& OutHitResults) const;
};
