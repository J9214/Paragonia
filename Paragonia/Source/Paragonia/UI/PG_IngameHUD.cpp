// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_IngameHUD.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Character/PGPlayerCharacterBase.h"
#include "UI/PG_PlayerHPBar.h"

void UPG_IngameHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    CheckOwnerPlayerAttribute();
}


bool UPG_IngameHUD::CheckOwnerPlayerAttribute()
{
    APlayerController* PlaterController = GetOwningPlayer();
    if (!PlaterController) return false;

    APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(PlaterController->GetPawn());
    if (!PlayerCharacter) return false;

    UCharacterAttributeSet* AttrSet = PlayerCharacter->GetCharacterAttributeSet();
    if (!AttrSet) return false;

    if (OwnerPlayerAttributeSet == AttrSet)
    {
        return true;
    }
    else if (!OwnerPlayerAttributeSet)
    {
        OwnerPlayerAttributeSet->OnHealthChanged.RemoveDynamic(this, &ThisClass::HandlePlayerHealthChanged);
        OwnerPlayerAttributeSet->OnMaxHealthChanged.RemoveDynamic(this, &ThisClass::HandlePlayerMaxHealthChanged);

        OwnerPlayerAttributeSet = nullptr;
    }

    OwnerPlayerAttributeSet = AttrSet;

    AttrSet->OnHealthChanged.AddUniqueDynamic(this, &UPG_IngameHUD::HandlePlayerHealthChanged);
    AttrSet->OnMaxHealthChanged.AddUniqueDynamic(this, &UPG_IngameHUD::HandlePlayerMaxHealthChanged);

    if (PlayerHPBar)
    {
        PlayerHPBar->HandleMaxHealthChanged(0, AttrSet->GetMaxHealth());
        PlayerHPBar->HandleHealthChanged(0, AttrSet->GetHealth());
    }

    return true;
}

void UPG_IngameHUD::HandlePlayerHealthChanged(float OldValue, float NewValue)
{
    if (!PlayerHPBar || !OwnerPlayerAttributeSet)
        return;

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

}

void UPG_IngameHUD::HandleTeam1MaxHealthChanged(float OldValue, float NewValue)
{

}

void UPG_IngameHUD::HandleTeam2HealthChanged(float OldValue, float NewValue)
{
    if (NewValue < OldValue)
    {
        //HP 달면 나오는 화면 연출 (팀 HP bar 쉐이킹 하는 애니, 재생 후 원위치)
    }
}

void UPG_IngameHUD::HandleTeam2MaxHealthChanged(float OldValue, float NewValue)
{
}
