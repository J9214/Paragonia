// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PGInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UPGInventoryComponent;

UCLASS()
class PARAGONIA_API UPGInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Init(UPGInventoryComponent* InInventory, int32 InSlotIndex);

    void Refresh();

    UPROPERTY(EditDefaultsOnly, Category = "DragDrop")
    TSubclassOf<UUserWidget> DragVisualWidgetClass;

protected:
    virtual void NativeConstruct() override;

    // Drag/Drop
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    void SetEmptyVisual();
   
protected:
    UPROPERTY(meta = (BindWidget)) UImage* IconImage;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* CountText;

    UPROPERTY() TObjectPtr<UPGInventoryComponent> Inventory;
    int32 SlotIndex = INDEX_NONE;

   
};