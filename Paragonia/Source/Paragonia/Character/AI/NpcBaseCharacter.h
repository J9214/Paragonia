// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NpcBaseCharacter.generated.h"

UCLASS()
class PARAGONIA_API ANpcBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANpcBaseCharacter();

	virtual void BeginPlay() override;

};
