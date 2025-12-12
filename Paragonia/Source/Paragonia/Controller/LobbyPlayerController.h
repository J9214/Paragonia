// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class UPG_LobbyWidget;
class UPG_CharacterSelectWidget;


/**
 *
 */
UCLASS()
class PARAGONIA_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(Category = UI)
	void ShowCharacterSelect();

	UFUNCTION(Category = UI)
	void ShowMatchStart();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess))
	TSubclassOf<UPG_LobbyWidget> MatchStartUIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess))
	TObjectPtr<UPG_LobbyWidget> MatchStartUIWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess))
	TSubclassOf<UPG_CharacterSelectWidget> CharSelectUIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess))
	TObjectPtr<UPG_CharacterSelectWidget> CharSelectUIWidgetInstance;
};
