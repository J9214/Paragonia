// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PG_CharacterSelectButton.generated.h"

class UImage;
class UTextBlock;
class UCharacterInfoWrapper;
class UPGStringTableSubsystem;
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

	UFUNCTION()
	void ApplySelectedVisual(UCharacterInfoWrapper* Warp);
protected:

	UPROPERTY()
	TObjectPtr<UPGStringTableSubsystem> StringTableSubsys;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> OutLineImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Cover;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor NormalColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor SelectedColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor TeamSelectedColor;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor MySelectedColor;

	UFUNCTION()
	void HandleCharacterHovered();

	UFUNCTION()
	void HandleCharacterUnHovered();

	UFUNCTION()
	void HandleCharacterSelected();

	int32 CharacterUID;
	bool bSelected;
	bool bTeamSelected;
	bool bPlayerSelected;
	bool bCheckCanSelected;
};
