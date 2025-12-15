// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PG_CharacterSelectWidget.generated.h"

class UCommonTileView;
class UButton;
class UPG_CharacterDescription;
class UPGCharacterDescriptionSubsystem;
class UPG_PlayerIcon;
class ALobbyPlayerState;
class APG_LobbyPreviewCharacter;
/**
 * 
 */

UCLASS()
class PARAGONIA_API UPG_CharacterSelectWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void SetInit();

	void SetPlayerCharacterIcon(int index, int CharacterUID);
	void SetPlayerReady(int index);

	UFUNCTION(BlueprintCallable)
	bool SpawnPreviewActorIfNeeded();

	void SetUI();
protected:

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnim;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTileView> CharacterTileView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_CharacterDescription> CharacterDescription;

	UPROPERTY(meta= (BindWidget))
	TObjectPtr<UPG_PlayerIcon> Player0Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_PlayerIcon> Player1Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_PlayerIcon> Player2Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY()
	TObjectPtr<ALobbyPlayerState> LobbyPlayerState;

	UPROPERTY()
	TObjectPtr<UPGCharacterDescriptionSubsystem> CharacterDescSubsys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
	TSubclassOf<APG_LobbyPreviewCharacter> PreviewActorClass;

	UPROPERTY()
	TObjectPtr<APG_LobbyPreviewCharacter> PreviewActorInstance;

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleCharacterItemClicked(UObject* Item);

	UFUNCTION()
	void HandlePlayerReadyClicked();

	UFUNCTION()
	void HandleAnyCharacterIDChanged(int32 NewCharacterID);

	UFUNCTION()
	void HandlePlayerSelected(EPlayerLobbyState LobbyState);

	UFUNCTION()
	void RefreshCharacterTileView();

	void InitCharacterList();

	bool CheckPlayerState();

	UPROPERTY(EditAnywhere, Category = "Description")
	FVector2D DescriptionPos;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	FRotator SpawnRotation;

	int32 SelectedCharacterUID;
};
