// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PGObject.generated.h"

UCLASS()
class PARAGONIA_API APGObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APGObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
