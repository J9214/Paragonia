#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PGGameplayAbilityBase.generated.h"

UENUM(BlueprintType)
enum class EPGAbilityInputID : uint8
{
	None,
	HitCheck,
	Jump,
	Attack,
	SkillQ,
	SkillE,
	SkillR
};

UCLASS()
class PARAGONIA_API UPGGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EPGAbilityInputID InputID = EPGAbilityInputID::None;
};
