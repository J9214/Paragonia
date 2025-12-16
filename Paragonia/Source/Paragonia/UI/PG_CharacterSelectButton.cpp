// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterSelectButton.h"
#include "Struct/CharacterInfoWrapper.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerState/LobbyPlayerState.h"
#include "GameState/LobbyGameStateBase.h"
#include "Subsystem/PGStringTableSubsystem.h"

void UPG_CharacterSelectButton::NativeOnListItemObjectSet(UObject* ListItemObject)
{

    if (UGameInstance* GI = GetGameInstance())
    {
        StringTableSubsys = GI->GetSubsystem<UPGStringTableSubsystem>();
    }

    if ( !StringTableSubsys)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterDescSubsys is null (PGStringTableSubsystem)."));
        return;
    }

    auto* Entry = Cast<UCharacterInfoWrapper>(ListItemObject);
    if (!Entry)
        return;

    const FCharacterDescription& Desc = Entry->DescriptionData;
    const FCharacterResourceInfo& Resorce = Entry->ResourceData;

    if (IconImage)
    {
        UTexture2D* Tex = nullptr;

        if (Resorce.BoxIcon.IsValid())
            Tex = Resorce.BoxIcon.Get();
        else
            Tex = Resorce.BoxIcon.LoadSynchronous();

        IconImage->SetBrushFromTexture(Tex, true);
    }

    if (Name)
        Name->SetText(StringTableSubsys->GetText(ELanguageType::English, Desc.DisplayName));

    if(BackgroundImage)
        BackgroundImage->SetColorAndOpacity(NormalColor);

    if(OutLineImage)
        OutLineImage->SetOpacity(0.0f);

    CharacterUID = Desc.UID;

    ApplySelectedVisual(Entry);
}

void UPG_CharacterSelectButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->OnHovered().AddUObject(this, &ThisClass::HandleCharacterHovered);

    this->OnUnhovered().AddUObject(this, &ThisClass::HandleCharacterUnHovered);

    this->OnClicked().AddUObject(this, &ThisClass::HandleCharacterSelected);
}

void UPG_CharacterSelectButton::HandleCharacterHovered()
{
    if (!BackgroundImage || !OutLineImage)
        return;

    if (bSelected || bPlayerSelected || bTeamSelected)
        return;

    BackgroundImage->SetColorAndOpacity(HoverColor);
    OutLineImage->SetOpacity(1.0f);
    OutLineImage->SetColorAndOpacity(HoverColor);
}

void UPG_CharacterSelectButton::HandleCharacterUnHovered()
{
    if (!BackgroundImage || !OutLineImage)
        return;

    if (bSelected || bPlayerSelected || bTeamSelected)
        return;

    BackgroundImage->SetColorAndOpacity(NormalColor);
    OutLineImage->SetOpacity(0.0f);
}

void UPG_CharacterSelectButton::HandleCharacterSelected()
{
    if (!BackgroundImage || !OutLineImage)
        return;

    BackgroundImage->SetColorAndOpacity(SelectedColor);
    OutLineImage->SetOpacity(1.0f);
    OutLineImage->SetColorAndOpacity(SelectedColor);
}

void UPG_CharacterSelectButton::ApplySelectedVisual(UCharacterInfoWrapper* Warp)
{
    bTeamSelected = Warp->bTeamSelected;
    bSelected = Warp->bSelected;
    bPlayerSelected = Warp->bPlayerSelected;
    bCheckCanSelected = Warp->bCheckCanSelected;

    if (!bCheckCanSelected)
    {
        Cover->SetOpacity(0.6f);
        this->SetIsEnabled(false);
    }

    if (bPlayerSelected)
    {
        BackgroundImage->SetColorAndOpacity(MySelectedColor);
        OutLineImage->SetColorAndOpacity(MySelectedColor);
        OutLineImage->SetOpacity(1.0f);
        return;
    }

    if (bSelected)
    {
        BackgroundImage->SetColorAndOpacity(SelectedColor);
        OutLineImage->SetColorAndOpacity(SelectedColor);
        OutLineImage->SetOpacity(1.0f);
        return;
    }

    if (bTeamSelected)
    {
        BackgroundImage->SetColorAndOpacity(TeamSelectedColor);
        OutLineImage->SetColorAndOpacity(TeamSelectedColor);
        OutLineImage->SetOpacity(1.0f);
        return;
    }

    BackgroundImage->SetColorAndOpacity(NormalColor);
    OutLineImage->SetOpacity(0.0f);

}

