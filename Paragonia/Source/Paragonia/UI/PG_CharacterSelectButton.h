// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PG_CharacterSelectButton.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_CharacterSelectButton : public UCommonButtonBase, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeOnInitialized() override;

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor NormalColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor TeamSelectedColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor MySelectedColor;

	bool Selected = false;


	UFUNCTION()
	void HandleCharacterHovered();

	UFUNCTION()
	void HandleCharacterUnHovered();
};
