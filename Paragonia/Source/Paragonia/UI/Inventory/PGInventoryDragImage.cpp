// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/PGInventoryDragImage.h"

void UPGInventoryDragImage::SetItemImage(UTexture2D* NewTexture)
{
    if (ItemImage && NewTexture)
    {
        ItemImage->SetBrushFromTexture(NewTexture, true);
    }
}