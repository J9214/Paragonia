#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PGShopItemEntryWidget.generated.h"

class UImage;
class UBorder;

UCLASS()
class PARAGONIA_API UPGShopItemEntryWidget : public UUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

protected:
    UPROPERTY(meta = (BindWidget)) 
    UImage* IconImage;

    UPROPERTY(meta = (BindWidget))
    UBorder* SelectionBorder;
};
