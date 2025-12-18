// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  
#include "FCharacterDescription.generated.h"

USTRUCT(BlueprintType)
struct FCharacterDescription : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 UID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill1Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill1Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill2Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill2Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill3Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Skill3Description;

};

