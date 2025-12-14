// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct/FCharacterDescription.h"
#include "CharacterDescriptionWrapper.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API UCharacterDescriptionWrapper : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName RowName;

	UPROPERTY()
	FCharacterDescription Data;

	UPROPERTY()
	bool bSelected = false;

	UPROPERTY()
	bool bTeamSelected = false;

	UPROPERTY()
	bool bPlayerSelected = false;

	UPROPERTY()
	bool bCheckCanSelected = true;
};
