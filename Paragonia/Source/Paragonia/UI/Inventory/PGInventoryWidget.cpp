#include "UI/Inventory/PGInventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

#include "Inventory/PGInventoryComponent.h" 
#include "UI/Inventory/PGInventorySlotWidget.h"

#include "PlayerState/PGPlayerState.h"

void UPGInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitFromOwningPlayer();
}

void UPGInventoryWidget::NativeDestruct()
{
    if (Inventory)
    {
        Inventory->OnInventoryChanged.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void UPGInventoryWidget::InitFromOwningPlayer()
{
    APGPlayerState* PS = Cast<APGPlayerState>(GetOwningPlayerState());
    if (!PS) return;

    Inventory = PS->FindComponentByClass<UPGInventoryComponent>();
    if (!Inventory) return;

    Inventory->OnInventoryChanged.RemoveAll(this);
    Inventory->OnInventoryChanged.AddUObject(this, &UPGInventoryWidget::RefreshAll);
    
    if (SlotWidgets.Num() == 0)
    {
        BuildSlots();
    }
    RefreshAll();
    
}

void UPGInventoryWidget::BindInventory(UPGInventoryComponent* InInventory)
{
    Inventory = InInventory;

    RefreshAll();
}

void UPGInventoryWidget::BuildSlots()
{
    if (!SlotWidgetClass)
    {
        SlotWidgetClass = UPGInventorySlotWidget::StaticClass();
    }

    if (!SlotGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("BuildSlots FAILED: SlotGrid is null. (BindWidget name mismatch?)"));
        return;
    }

    if (!SlotWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BuildSlots FAILED: SlotWidgetClass is null."));
        return;
    }

    SlotGrid->ClearChildren();
    SlotWidgets.Reset();

    const int32 Num = Inventory->MaxSlots; 
    const int32 Columns = 3;

    for (int32 i = 0; i < Num; ++i)
    {
        UPGInventorySlotWidget* SlotW = CreateWidget<UPGInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
        if (!SlotW)
        {
            UE_LOG(LogTemp, Error, TEXT("BuildSlots: CreateWidget failed at %d"), i);
            continue;
        }

        SlotW->Init(Inventory, i);

        SlotGrid->AddChildToUniformGrid(SlotW, i / Columns, i % Columns);
        SlotWidgets.Add(SlotW);
    }

}

void UPGInventoryWidget::RefreshAll()
{
    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        UPGInventorySlotWidget* W = SlotWidgets[i];

        if (W)
        {
            W->Refresh();
        }
    }
}

