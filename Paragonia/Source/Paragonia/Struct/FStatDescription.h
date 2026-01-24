// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  
#include "FStatDescription.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PARAGONIA_API FStatDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> StatIcon;

};
