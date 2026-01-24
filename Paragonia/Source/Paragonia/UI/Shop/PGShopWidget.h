// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shop/PGShopTypes.h"
#include "PGShopWidget.generated.h"


class UTileView;
class UScrollBox;
class UButton;
class UTextBlock;
class UPGShopDetailWidget;
class UPGShopComponent;
class UPGShopItemObject;
class UPGInventoryWidget;

UCLASS()
class PARAGONIA_API UPGShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void InitWithShopComponent(UPGShopComponent* InShop);

protected:
    UFUNCTION()
    void HandleItemClicked(UObject* Item);

    void RebuildList();

    void BuildCategoryButtons();

    UFUNCTION()
    void OnBuyResult(EShopBuyResult Result, FName ItemId);

    UFUNCTION() void OnCategoryAll();
    UFUNCTION() void OnCategoryWeapon();
    UFUNCTION() void OnCategoryArmor();
    UFUNCTION() void OnCategoryConsumable();
    UFUNCTION() void OnCategoryEtc();

    UFUNCTION()
    void HandleInventorySlotSelected(int32 SlotIndex);
    UFUNCTION()
    void HandleInventorySlotRightClick(int32 SlotIndex);
    UFUNCTION()
    void HandleGoldChange(int32 NewGold);

protected:
    UPROPERTY(meta = (BindWidget)) 
    UTileView* ItemTileView;

    UPROPERTY(meta = (BindWidget)) 
    UScrollBox* CategoryScroll;

    UPROPERTY(meta = (BindWidget)) 
    UPGShopDetailWidget* DetailWidget;

    UPROPERTY(meta = (BindWidgetOptional))
    UPGInventoryWidget* PlayerInventoryWidget;

    UPROPERTY(meta = (BindWidgetOptional)) 
    UTextBlock* GoldText;

    UPROPERTY() 
    TObjectPtr<UPGShopComponent> ShopComp;

    UPROPERTY()
    EShopCategory CurrentCategory = EShopCategory::All;

    UPROPERTY()
    EShopCategory Category;
};