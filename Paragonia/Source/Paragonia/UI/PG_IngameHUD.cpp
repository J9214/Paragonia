// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_IngameHUD.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Character/PGPlayerCharacterBase.h"
#include "UI/PG_HPBar.h"

void UPG_IngameHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UPG_IngameHUD::HandlePlayerHealthChanged(float OldValue, float NewValue)
{
    if (NewValue < OldValue)
    {
        //HP 달면 나오는 화면 연출 (image 투명도 조절)
    }
    PlayerHPBar->HandleHealthChanged(OldValue, NewValue);
}

void UPG_IngameHUD::HandlePlayerMaxHealthChanged(float OldValue, float NewValue)
{
    PlayerHPBar->HandleMaxHealthChanged(OldValue, NewValue);
}

void UPG_IngameHUD::HandleTeam1HealthChanged(float OldValue, float NewValue)
{
    if (NewValue < OldValue)
    {
        //HP 달면 나오는 화면 연출 (팀 HP bar 쉐이킹 하는 애니, 재생 후 원위치)
    }
    Team1HPBar->HandleHealthChanged(OldValue, NewValue);
}

void UPG_IngameHUD::HandleTeam1MaxHealthChanged(float OldValue, float NewValue)
{
    Team1HPBar->HandleMaxHealthChanged(OldValue, NewValue);
}

void UPG_IngameHUD::HandleTeam2HealthChanged(float OldValue, float NewValue)
{
    if (NewValue < OldValue)
    {
        //HP 달면 나오는 화면 연출 (팀 HP bar 쉐이킹 하는 애니, 재생 후 원위치)
    }

    Team2HPBar->HandleHealthChanged(OldValue, NewValue);
}

void UPG_IngameHUD::HandleTeam2MaxHealthChanged(float OldValue, float NewValue)
{
    Team2HPBar->HandleMaxHealthChanged(OldValue, NewValue);
}
