// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PGShopTypes.h"
#include "PGShopCatalog.generated.h"

UCLASS(BlueprintType)
class PARAGONIA_API UPGShopCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FPGShopItemRow> Items;
};
