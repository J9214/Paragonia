// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"  
#include "PaperSprite.h"
#include "FCharacterResourceInfo.generated.h"

USTRUCT(BlueprintType)
struct PARAGONIA_API FCharacterResourceInfo : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 UID = -1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> BoxIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> CircleIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UPaperSprite> MinimapIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> LobbyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<UAnimInstance> LobbyAnimBP;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> IntroMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Skill1Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Skill2Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UAnimMontage> Skill3Montage;
};
