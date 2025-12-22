#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct/FAttackData.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "PGGreystoneSkillAura.generated.h"

class UAbilitySystemComponent;

UCLASS()
class PARAGONIA_API APGGreystoneSkillAura : public AActor
{
	GENERATED_BODY()
	
public:	
	APGGreystoneSkillAura();

	void InitFromAbility(AActor* InSourceActor, const FAttackData& InAttackData);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void StartAura();

	void TickAura_SendHitCheck();

	void OnHitResultEvent(const FGameplayEventData Payload);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Greystone|SkillAura")
	float TickInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Greystone|SkillAura")
	float LifeTime;

private:
	UPROPERTY()
	TObjectPtr<AActor> SourceActor;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	FAttackData AttackData;

	FTimerHandle TickTimerHandle;
};
