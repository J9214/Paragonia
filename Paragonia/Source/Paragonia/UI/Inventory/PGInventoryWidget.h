#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PGInventoryWidget.generated.h"

class UUniformGridPanel;
class UPGInventoryComponent;
class UPGInventorySlotWidget;

UCLASS()
class PARAGONIA_API UPGInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable)
    void InitFromOwningPlayer(); 

    UFUNCTION(BlueprintCallable)
    void BindInventory(UPGInventoryComponent* InInventory);

protected:
    void BuildSlots();

    void RefreshAll();

protected:
    UPROPERTY(meta = (BindWidget)) 
    UUniformGridPanel* SlotGrid;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryUI")
    TSubclassOf<UPGInventorySlotWidget> SlotWidgetClass;

    UPROPERTY() 
    TObjectPtr<UPGInventoryComponent> Inventory;

    UPROPERTY() 
    TArray<TObjectPtr<UPGInventorySlotWidget>> SlotWidgets;
};
