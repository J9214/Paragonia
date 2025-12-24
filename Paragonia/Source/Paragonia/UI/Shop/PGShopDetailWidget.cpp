#include "UI/Shop/PGShopDetailWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"

#include "Shop/PGShopComponent.h"

void UPGShopDetailWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BuyButton)
    {
        BuyButton->OnClicked.AddDynamic(this, &UPGShopDetailWidget::OnBuyClicked);
        SellButton->OnClicked.AddDynamic(this, &UPGShopDetailWidget::OnSellClicked);
    }

    if (CountSpin)
    {
        CountSpin->SetMinValue(1);
        CountSpin->SetMaxValue(99);
        CountSpin->SetValue(1);
    }
}

void UPGShopDetailWidget::SetItem(const FPGShopItemRow& InItem)
{
    CurrentItem = InItem;

    if (NameText)  
    {
        NameText->SetText(InItem.DisplayName);
    }
    if (DescText)  
    {
        DescText->SetText(InItem.Description);
    }
    if (PriceText)
    {
        PriceText->SetText(FText::AsNumber(InItem.Price));
    }

    if (IconImage)
    {
        if (InItem.Icon.IsNull())
        {
            IconImage->SetBrushFromTexture(nullptr);
        }
        else
        {
            UTexture2D* Tex = InItem.Icon.LoadSynchronous();
            IconImage->SetBrushFromTexture(Tex);
        }
    }

    RefreshStock();
}

void UPGShopDetailWidget::RefreshStock()
{
    if (!StockText)
    {
        return;
    }

    if (!ShopComp)
    {
        StockText->SetText(FText::FromString(TEXT("-")));
        return;
    }

    const int32 Remaining = ShopComp->GetRemainingStock(CurrentItem.ItemId);
    StockText->SetText(FText::FromString(FString::Printf(TEXT("재고: %d"), Remaining)));
}

void UPGShopDetailWidget::OnBuyClicked()
{
    if (!ShopComp) 
    {
        return;
    }

    ShopComp->RequestBuy(CurrentItem.ItemId);
}

void UPGShopDetailWidget::OnSellClicked()
{
    if (!ShopComp)
    {
        return;
    }

    ShopComp->RequestSell(CurrentSlotIndex);
}
