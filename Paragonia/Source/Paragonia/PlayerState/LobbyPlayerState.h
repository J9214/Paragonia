// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerLobbyState : uint8
{
	PLS_NotReady,
	PLS_MatchingReady,
	PLS_Selecting,
	PLS_SelectedAndReady,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyPlayerStateChangedDelegate, EPlayerLobbyState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterIDChangedDelegate, int32, NewCharacterID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamIDChangedDelegate, int32, NewTeamID);

/**
 *
 */
UCLASS()
class PARAGONIA_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CopyProperties(APlayerState* PlayerState) override;

public:
	UFUNCTION(BlueprintCallable, Category = "LobbyData")
	EPlayerLobbyState GetPlayerLobbyState() const { return PlayerLobbyState; }

	UFUNCTION(BlueprintCallable, Category = "LobbyData")
	int32 GetCharacterID() const { return CharacterID; }

	UFUNCTION(BlueprintCallable, Category = "LobbyData")
	int32 GetTeamID() const { return TeamID; }

	// Client -> Server RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLobbyState(EPlayerLobbyState NewState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCharacterID(int32 NewID);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTeamID(int32 NewTeamID);

public:
	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLobbyPlayerStateChangedDelegate OnLobbyPlayerStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterIDChangedDelegate OnCharacterIDChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTeamIDChangedDelegate OnTeamIDChanged;
protected:
	UFUNCTION()
	void OnRep_PlayerLobbyState();

	UFUNCTION()
	void OnRep_CharacterID();

	UFUNCTION()
	void OnRep_TeamID();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerLobbyState)
	EPlayerLobbyState PlayerLobbyState;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterID)
	int32 CharacterID;

	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	int32 TeamID;
};
