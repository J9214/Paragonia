// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/PGPlayerState.h"
#include "GameMode/LobbyGameModeBase.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerLobbyState);
	DOREPLIFETIME(ThisClass, CharacterID);
	DOREPLIFETIME(ThisClass, TeamID);
}

void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	APGPlayerState* TargetPS = Cast<APGPlayerState>(PlayerState);
	if (TargetPS)
	{
		// 차후 수정 예정
	}
}

void ALobbyPlayerState::ServerSetLobbyState_Implementation(EPlayerLobbyState NewState)
{
	PlayerLobbyState = NewState;

	OnRep_PlayerLobbyState();

	if (UWorld* World = GetWorld())
	{
		if (ALobbyGameModeBase* GM = Cast<ALobbyGameModeBase>(World->GetAuthGameMode()))
		{
			if (PlayerLobbyState == EPlayerLobbyState::PLS_MatchingReady)
			{
				GM->CheckStartingCondition();
			}
			else if (PlayerLobbyState == EPlayerLobbyState::PLS_SelectedAndReady)
			{
				GM->CheckAllPlayersReady();
			}
		}
	}
}

bool ALobbyPlayerState::ServerSetLobbyState_Validate(EPlayerLobbyState NewState)
{
	if(NewState == PlayerLobbyState)
		return false;

	return true;
}

void ALobbyPlayerState::ServerSetCharacterID_Implementation(int32 NewID)
{
	CharacterID = NewID;

	OnRep_CharacterID();
}

bool ALobbyPlayerState::ServerSetCharacterID_Validate(int32 NewID)
{
	if (NewID < 0)
		return false;

	if (NewID == CharacterID)
		return false;

	return true;
}

void ALobbyPlayerState::ServerSetTeamID_Implementation(int32 NewTeamID)
{
	TeamID = NewTeamID;

	OnRep_TeamID();
}

bool ALobbyPlayerState::ServerSetTeamID_Validate(int32 NewTeamID)
{
	if (NewTeamID < 0)
		return false;

	if (NewTeamID == TeamID)
		return false;

	return true;
}

void ALobbyPlayerState::OnRep_PlayerLobbyState()
{
	if (OnLobbyPlayerStateChanged.IsBound())
	{
		OnLobbyPlayerStateChanged.Broadcast(PlayerLobbyState);
	}
}

void ALobbyPlayerState::OnRep_CharacterID()
{
	if (OnCharacterIDChanged.IsBound())
	{
		OnCharacterIDChanged.Broadcast(CharacterID);
	}
}

void ALobbyPlayerState::OnRep_TeamID()
{
	if (OnTeamIDChanged.IsBound())
	{
		OnTeamIDChanged.Broadcast(TeamID);
	}
}
