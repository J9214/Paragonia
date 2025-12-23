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

protected:
    UFUNCTION()
    void OnBuyClicked();

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

    UPROPERTY() 
    TObjectPtr<UPGShopComponent> ShopComp;

    UPROPERTY()
    FPGShopItemRow CurrentItem;
};
