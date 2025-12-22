// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panels/PG_IngameInfo.h"
#include "UI/Bars/PG_HPBar.h"

void UPG_IngameInfo::HandleHealthChanged(float OldValue, float NewValue)
{
	if (PlayerHPBar)
	{
		PlayerHPBar->HandleHealthChanged(OldValue, NewValue);
	}
}


void UPG_IngameInfo::HandleMaxHealthChanged(float OldValue, float NewValue)
{
	if (PlayerHPBar)
	{
		PlayerHPBar->HandleMaxHealthChanged(OldValue, NewValue);
	}
}