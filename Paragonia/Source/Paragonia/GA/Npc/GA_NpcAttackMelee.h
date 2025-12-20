// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/Npc/GA_NpcAttackBase.h"
#include "GA_NpcAttackMelee.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API UGA_NpcAttackMelee : public UGA_NpcAttackBase
{
	GENERATED_BODY()

protected:
	virtual void OnAttackEventReceived(FGameplayEventData Payload) override;
};
