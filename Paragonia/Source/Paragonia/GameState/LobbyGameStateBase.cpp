// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/LobbyGameStateBase.h"
#include "Net/UnrealNetwork.h"

ALobbyGameStateBase::ALobbyGameStateBase()
	:LeftTime(0)
{
	CurrentLobbyState = EGameLobbyState::GLS_WaitingForPlayers;
}

void ALobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentLobbyState);
	DOREPLIFETIME(ThisClass, LeftTime);
}

void ALobbyGameStateBase::SetLobbyState(EGameLobbyState NewState)
{
	if (HasAuthority())
	{
		CurrentLobbyState = NewState;
		OnRep_CurrentLobbyState();
	}
}

void ALobbyGameStateBase::SetLeftTime(int32 NewTime)
{
	if (HasAuthority())
	{
		LeftTime = NewTime;
		OnRep_LeftTime();
	}
}

void ALobbyGameStateBase::OnRep_CurrentLobbyState()
{
	if (OnLobbyGameStateChanged.IsBound())
	{
		OnLobbyGameStateChanged.Broadcast(CurrentLobbyState);
	}
}

void ALobbyGameStateBase::OnRep_LeftTime()
{
	if (OnLeftTimeChanged.IsBound())
	{
		OnLeftTimeChanged.Broadcast(LeftTime);
	}
}
