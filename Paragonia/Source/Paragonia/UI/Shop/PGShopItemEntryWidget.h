#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PGShopItemEntryWidget.generated.h"

class UImage;

UCLASS()
class PARAGONIA_API UPGShopItemEntryWidget : public UUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
    UPROPERTY(meta = (BindWidget)) 
    UImage* IconImage;
};
