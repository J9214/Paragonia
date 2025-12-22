// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDs/PG_IngameHUD.h"
#include "UI/Panels/PG_InGameTeamSimpleInfo.h"
#include "UI/Bars/PG_HPBar.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Character/PGPlayerCharacterBase.h"
#include "UI/MiniMap/PG_MiniMap.h"

void UPG_IngameHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UPG_IngameHUD::InitMinimap(UTextureRenderTarget2D* InRT)
{
    if (!MiniMapWidget || !InRT)
    {
        return;
    }

    MiniMapWidget->SetMinimapRenderTarget(InRT);
}

void UPG_IngameHUD::InitTeam1IngameIcon(int32 CharacterID)
{
    Team1HPBar->InitTeamSimpleInfo(CharacterID);
}

void UPG_IngameHUD::InitTeam2IngameIcon(int32 CharacterID)
{
    Team2HPBar->InitTeamSimpleInfo(CharacterID);
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
