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
	FCharacterDescription Data;
};
