// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PGInventoryDragImage.generated.h"

UCLASS()
class PARAGONIA_API UPGInventoryDragImage : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetItemImage(UTexture2D* NewTexture);
    
private:
    UPROPERTY(meta = (BindWidget))
    UImage* ItemImage;
};
