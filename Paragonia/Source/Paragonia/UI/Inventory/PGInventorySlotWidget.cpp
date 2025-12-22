#include "UI/Inventory/PGInventorySlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "UI/Inventory/PGInventoryDragOp.h"
#include "Inventory/PGInventoryComponent.h"  

#include "Shop/PGShopTypes.h" 
#include "Engine/DataTable.h"

void UPGInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPGInventorySlotWidget::Init(UPGInventoryComponent* InInventory, int32 InSlotIndex)
{
    Inventory = InInventory;
    SlotIndex = InSlotIndex;
    Refresh();
}

void UPGInventorySlotWidget::Refresh()
{
    if (!Inventory || SlotIndex == INDEX_NONE)
    {
        SetEmptyVisual();
        return;
    }

    if (!Inventory->Slots.IsValidIndex(SlotIndex))
    {
        SetEmptyVisual();
        return;
    }

    const FInventorySlot& InvSlot = Inventory->Slots[SlotIndex];

    if (IconImage)
    {
        if (!InvSlot.bEmpty && !InvSlot.ItemId.IsNone())
        {
            UDataTable* DT = Inventory->GetItemDataTable();
            if (DT)
            {
                static const FString Context(TEXT("InventorySlotIcon"));
                const FPGShopItemRow* Row = DT->FindRow<FPGShopItemRow>(InvSlot.ItemId, Context);

                UTexture2D* IconTex = Row ? Row->Icon.LoadSynchronous() : nullptr;

                if (IconTex)
                {
                    IconImage->SetBrushFromTexture(IconTex);
                    IconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                }
                else
                {
                    IconImage->SetBrushFromTexture(nullptr);
                    IconImage->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }
        else
        {
            IconImage->SetBrushFromTexture(nullptr);
            IconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (CountText)
    {
        CountText->SetText(FText::AsNumber(InvSlot.Count));
        CountText->SetVisibility(InvSlot.Count > 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UPGInventorySlotWidget::SetEmptyVisual()
{
    if (IconImage)
    {
        IconImage->SetBrushFromTexture(nullptr);
    }
    if (CountText)
    {
        CountText->SetText(FText::GetEmpty());
        CountText->SetVisibility(ESlateVisibility::Hidden);
    }
}

FReply UPGInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UPGInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!Inventory || !Inventory->Slots.IsValidIndex(SlotIndex))
        return;

    const FInventorySlot& InvSlot = Inventory->Slots[SlotIndex];
    if (InvSlot.bEmpty)
        return;

    UPGInventoryDragOp* Op = NewObject<UPGInventoryDragOp>();
    Op->FromIndex = SlotIndex;
    Op->Pivot = EDragPivot::MouseDown;

    OutOperation = Op;
}

bool UPGInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UPGInventoryDragOp* Op = Cast<UPGInventoryDragOp>(InOperation);
    if (!Op || !Inventory) return false;

    const int32 From = Op->FromIndex;
    const int32 To = SlotIndex;

    if (From == INDEX_NONE || To == INDEX_NONE) return false;
    if (From == To) return true;

    Inventory->RequestSwapSlot(From, To);
    return true;
}
