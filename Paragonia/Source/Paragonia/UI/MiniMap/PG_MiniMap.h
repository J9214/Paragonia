// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_MiniMap.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_MiniMap : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	void SetMinimapRenderTarget(UTextureRenderTarget2D* InRT);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MiniMapImage;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TObjectPtr<UMaterialInterface> MinimapUIMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MinimapMID;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> MinimapRT;
};
