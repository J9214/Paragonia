// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameState/LobbyGameStateBase.h"
#include "LobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONIA_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALobbyGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
public:
	void CheckAllPlayersReady();
	void CheckStartingCondition();

protected:
	void StartGameCountdown();
	void OnCountdownTimer();
	void GameLevelTravel();

	UFUNCTION()
	void OnPSLobbyStateChangedHandler(EPlayerLobbyState NewPlayerState);

	int32 GetCurrentPlayerTeamCount(int32 TeamIDToCheck, APlayerState* ExcludePS);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LobbySetting")
	int32 PlayerCountToStart;

	UPROPERTY(EditDefaultsOnly, Category = "LobbySetting")
	int32 CountdownDuration;

	int32 PlayerIDCounter;

protected:
	FTimerHandle CountdownTimerHandle;
};
