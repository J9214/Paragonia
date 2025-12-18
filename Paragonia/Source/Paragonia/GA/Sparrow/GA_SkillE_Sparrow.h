#pragma once

#include "CoreMinimal.h"
#include "GA/PGTargetingGameplayAbility.h"
#include "Struct/FAttackData.h"
#include "GA_SkillE_Sparrow.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PARAGONIA_API UGA_SkillE_Sparrow : public UPGTargetingGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SkillE_Sparrow();
};
