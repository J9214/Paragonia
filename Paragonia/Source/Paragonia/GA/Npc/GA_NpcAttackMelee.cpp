// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Npc/GA_NpcAttackMelee.h"

void UGA_NpcAttackMelee::OnAttackEventReceived(FGameplayEventData Payload)
{
	ApplyAttackDataEffects_OnHit(AttackData, Payload);
}