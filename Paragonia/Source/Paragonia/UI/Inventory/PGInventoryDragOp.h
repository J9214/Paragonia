// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "PGInventoryDragOp.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API UPGInventoryDragOp : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	int32 FromIndex = INDEX_NONE;
};
