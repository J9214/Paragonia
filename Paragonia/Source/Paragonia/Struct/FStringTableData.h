// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FStringTableData.generated.h"

UENUM(BlueprintType)
enum class ELanguageType : uint8
{
    Korean,
    English,
};
/**
 * 
 */
USTRUCT(BlueprintType)
struct PARAGONIA_API FStringTableData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText English;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Korean;
};
