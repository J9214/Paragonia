// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PGTeamStatusInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPGTeamStatusInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARAGONIA_API IPGTeamStatusInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Team")
	int32 GetTeamID() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Life")
	bool GetIsDead() const;
};
