// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PG_LobbyPreviewCharacter.generated.h"

UCLASS()
class PARAGONIA_API APG_LobbyPreviewCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APG_LobbyPreviewCharacter();

    UFUNCTION(BlueprintCallable)
    void SetCharacterMesh(USkeletalMesh* NewMesh);

    UFUNCTION(BlueprintCallable)
    void PlayIntroMontage(UAnimMontage* Montage);

protected:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* MeshComp;

};
