// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniMap/PG_MiniMap.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

void UPG_MiniMap::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UPG_MiniMap::SetMinimapRenderTarget(UTextureRenderTarget2D* InRT)
{
    if (!MiniMapImage || !MinimapUIMaterial || !InRT)
        return;

    MinimapRT = InRT;

    if (!MinimapMID)
    {
        MinimapMID = UMaterialInstanceDynamic::Create(MinimapUIMaterial, this);
        MiniMapImage->SetBrushFromMaterial(MinimapMID);
    }
     
    MinimapMID->SetTextureParameterValue(TEXT("MiniMapTex"), MinimapRT);
}
