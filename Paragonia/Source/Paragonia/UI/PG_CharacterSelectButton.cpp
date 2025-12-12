// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterSelectButton.h"
#include "Struct/CharacterDescriptionWrapper.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPG_CharacterSelectButton::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    //CheckSelected();

    auto* Entry = Cast<UCharacterDescriptionWrapper>(ListItemObject);
    if (!Entry)
        return;

    const FCharacterDescription& Desc = Entry->Data;

    if (IconImage)
    {
        UTexture2D* Tex = nullptr;

        if (Desc.BoxIcon.IsValid())
            Tex = Desc.BoxIcon.Get();
        else
            Tex = Desc.BoxIcon.LoadSynchronous();

        IconImage->SetBrushFromTexture(Tex, true);
    }

    if (Name)
    {
        Name->SetText(Desc.DisplayName);
    }
}

void UPG_CharacterSelectButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->OnHovered().AddUObject(
        this, &ThisClass::HandleCharacterHovered);

    this->OnUnhovered().AddUObject(
        this, &ThisClass::HandleCharacterUnHovered);
}

void UPG_CharacterSelectButton::HandleCharacterHovered()
{
    if (!BackgroundImage || Selected)
        return;

    BackgroundImage->SetColorAndOpacity(HoverColor);
}

void UPG_CharacterSelectButton::HandleCharacterUnHovered()
{
    if (!BackgroundImage || Selected)
        return;

    BackgroundImage->SetColorAndOpacity(NormalColor);
}
