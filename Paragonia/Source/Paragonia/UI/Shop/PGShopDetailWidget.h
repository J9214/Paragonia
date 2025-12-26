#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shop/PGShopTypes.h"
#include "PGShopDetailWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class USpinBox;
class UPGShopComponent;
class UPGInventorySlotWidget;

UCLASS()
class PARAGONIA_API UPGShopDetailWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void BindShop(UPGShopComponent* InShop) { ShopComp = InShop; }

    UFUNCTION(BlueprintCallable)
    void SetItem(const FPGShopItemRow& InItem);

    UFUNCTION(BlueprintCallable)
    void RefreshStock();

	UFUNCTION(BlueprintCallable)
    void SetCurrentSlotIndex(int32 Index) { CurrentSlotIndex = Index; }
protected:
    UFUNCTION()
    void OnBuyClicked();
    UFUNCTION()
    void OnSellClicked();

protected:
    UPROPERTY(meta = (BindWidget)) 
    UImage* IconImage;
    UPROPERTY(meta = (BindWidget)) 
    UTextBlock* NameText;
    UPROPERTY(meta = (BindWidget)) 
    UTextBlock* DescText;
    UPROPERTY(meta = (BindWidget)) 
    UTextBlock* PriceText;
    UPROPERTY(meta = (BindWidgetOptional)) 
    UTextBlock* StockText;

    UPROPERTY(meta = (BindWidgetOptional)) 
    USpinBox* CountSpin;
    UPROPERTY(meta = (BindWidget)) 
    UButton* BuyButton;
    UPROPERTY(meta = (BindWidget))
    UButton* SellButton;

    UPROPERTY() 
    TObjectPtr<UPGShopComponent> ShopComp;

    UPROPERTY()
    FPGShopItemRow CurrentItem;
    UPROPERTY()
    int32 CurrentSlotIndex;
};
