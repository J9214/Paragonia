// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Struct/FCharacterDescription.h"
#include "PG_CharacterDescription.generated.h"

class UButton;
class UTextBlock;
class UPGStringTableSubsystem;

/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_CharacterDescription : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void InitDescription(const FCharacterDescription& InDesc);

protected:

	UPROPERTY()
	TObjectPtr<UPGStringTableSubsystem> StringTableSubsys;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Skill1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Skill2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Skill3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDescription;

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void SetSkill1Description();

	UFUNCTION()
	void SetSkill2Description();

	UFUNCTION()
	void SetSkill3Description();

	UPROPERTY()
	FCharacterDescription CurrentDesc;

};
