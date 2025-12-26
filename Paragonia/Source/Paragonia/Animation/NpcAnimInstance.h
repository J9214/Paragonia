// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "NpcAnimInstance.generated.h"

class ANpcBaseCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class PARAGONIA_API UNpcAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UNpcAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Npc|Stat")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Npc|Stat")
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Npc|Stat")
	bool bIsDead;

	UFUNCTION()
	void AnimNotify_AttackHit();

	UFUNCTION()
	void AnimNotify_ProjectileFire();

private:
	void SendGameplayEventToOwner(FGameplayTag EventTag);

private:
	UPROPERTY()
	TObjectPtr<ANpcBaseCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
