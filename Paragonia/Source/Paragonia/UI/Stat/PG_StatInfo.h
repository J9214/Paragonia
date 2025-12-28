// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_StatInfo.generated.h"

class UImage;
class UTextBlock;
class UPGCharacterDescriptionSubsystem;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_StatInfo : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	bool SetStat(const FName& StatName, float Value);

	void ChangeValue(float Value);

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY()
	TObjectPtr<UPGCharacterDescriptionSubsystem> CharacterDescSubsys;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatValue;
};
