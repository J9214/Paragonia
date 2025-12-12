// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PG_CharacterSelectWidget.generated.h"

class UCommonTileView;
class UPG_CharacterDescription;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_CharacterSelectWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTileView> CharacterTileView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_CharacterDescription> CharacterDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UDataTable> CharacterDataTable;

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleCharacterItemClicked(UObject* Item);

	void InitCharacterList();

	UPROPERTY(EditAnywhere, Category = "Description")
	FVector2D DescriptionPos;

};
