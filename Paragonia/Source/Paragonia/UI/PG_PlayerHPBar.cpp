// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_PlayerHPBar.h"
#include "Components/ProgressBar.h"

void UPG_PlayerHPBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();


}

void UPG_PlayerHPBar::HandleHealthChanged(float OldValue, float NewValue)
{
	NowHPValue = NewValue;
    HPBar->SetPercent(MaxHPValue > 0.f ? NewValue / MaxHPValue : 0.f);
}

void UPG_PlayerHPBar::HandleMaxHealthChanged(float OldValue, float NewValue)
{
	MaxHPValue = NewValue;
	HPBar->SetPercent(NewValue > 0.f ? NowHPValue / NewValue : 0.f);
}
