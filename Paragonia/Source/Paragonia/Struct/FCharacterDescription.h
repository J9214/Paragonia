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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> BoxIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> CircleIcon;

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

