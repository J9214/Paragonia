#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PGShopTypes.h"
#include "PGShopItemObject.generated.h"

UCLASS()
class PARAGONIA_API UPGShopItemObject : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FPGShopItemRow Data;
};
