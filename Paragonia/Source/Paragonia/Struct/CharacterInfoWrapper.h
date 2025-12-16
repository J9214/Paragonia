// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct/FCharacterDescription.h"
#include "Struct/FCharacterResourceInfo.h"
#include "CharacterInfoWrapper.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API UCharacterInfoWrapper : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName RowName;

	UPROPERTY()
	FCharacterDescription DescriptionData;

	UPROPERTY()
	FCharacterResourceInfo ResourceData;

	UPROPERTY()
	bool bSelected = false;

	UPROPERTY()
	bool bTeamSelected = false;

	UPROPERTY()
	bool bPlayerSelected = false;

	UPROPERTY()
	bool bCheckCanSelected = true;
};
