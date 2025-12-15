#pragma once

#include "CoreMinimal.h"
#include "GA/PGTargetingGameplayAbility.h"
#include "Struct/FAttackData.h"
#include "GA_SkillQ_Sparrow.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PARAGONIA_API UGA_SkillQ_Sparrow : public UPGTargetingGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SkillQ_Sparrow();

protected:
	virtual void CreateEffectActor() override;
};
