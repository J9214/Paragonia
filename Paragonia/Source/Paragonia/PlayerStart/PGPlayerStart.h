// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PGPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API APGPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int32 TeamID;
};
