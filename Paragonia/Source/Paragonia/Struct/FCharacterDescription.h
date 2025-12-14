// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCharacterDescription.generated.h"

USTRUCT(BlueprintType)
struct FCharacterDescription : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 UID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill1Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill1Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill2Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill2Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill3Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Skill3Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> BoxIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> CircleIcon;
};

