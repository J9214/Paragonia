#include "UI/Shop/PGShopItemEntryWidget.h"
#include "Components/Image.h"
#include "Shop/PGShopItemObject.h" 
#include "Components/Border.h"

void UPGShopItemEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    const UPGShopItemObject* Obj = Cast<UPGShopItemObject>(ListItemObject);
    if (!Obj) 
    {
        return;
    }

    if (IconImage)
    {
        if (Obj->Data.Icon.IsNull())
        {
            IconImage->SetBrushFromTexture(nullptr);
        }
        else
        {
            UTexture2D* Tex = Obj->Data.Icon.LoadSynchronous();
            IconImage->SetBrushFromTexture(Tex);
        }
    }

    if (SelectionBorder)
    {
        SelectionBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UPGShopItemEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
    if (SelectionBorder)
    {
        if (bIsSelected)
        {
            SelectionBorder->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            SelectionBorder->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
