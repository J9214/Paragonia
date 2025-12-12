// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterSelectWidget.h"
#include "CommonTileView.h"
#include "Struct/CharacterDescriptionWrapper.h"
#include "UI/PG_CharacterDescription.h"
#include "Components/CanvasPanelSlot.h"

void UPG_CharacterSelectWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitCharacterList();
    if (CharacterTileView)
    {
        CharacterTileView->OnItemClicked().AddUObject(this, &ThisClass::HandleCharacterItemClicked);
    }
}

void UPG_CharacterSelectWidget::HandleCharacterItemClicked(UObject* Item)
{
    if (!Item || !CharacterDataTable)
        return;

    UCharacterDescriptionWrapper* Entry = Cast<UCharacterDescriptionWrapper>(Item);
    if (!Entry)
        return;

    FName RowName = Entry->Data.Id;

    const FCharacterDescription* DetailRow =
        CharacterDataTable->FindRow<FCharacterDescription>(RowName, TEXT("CharacterSelect"));

    if (!DetailRow)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character row '%s' not found in CharacterDataTable"),
            *RowName.ToString());
        return;
    }

    CharacterDescription->InitDescription(*DetailRow);

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CharacterDescription->Slot))
    {
        const FVector2D CurrentPos = CanvasSlot->GetPosition();

        if (!CurrentPos.Equals(DescriptionPos, 0.1f))
        {
            CanvasSlot->SetPosition(DescriptionPos);
        }
    }
}

void UPG_CharacterSelectWidget::InitCharacterList()
{
    if (!CharacterTileView || !CharacterDataTable)
        return;

    TArray<UObject*> Items;

    const TMap<FName, uint8*>& RowMap = CharacterDataTable->GetRowMap();

    for (const TPair<FName, uint8*>& Pair : RowMap)
    {
        const FName RowName = Pair.Key;
        FCharacterDescription* Row = reinterpret_cast<FCharacterDescription*>(Pair.Value);
        if (!Row)
            continue;

        UCharacterDescriptionWrapper* Wrapper =
            NewObject<UCharacterDescriptionWrapper>(this); // Outer는 Widget

        Wrapper->Data = *Row;
        Items.Add(Wrapper);
    }

    CharacterTileView->SetListItems(Items);
}
