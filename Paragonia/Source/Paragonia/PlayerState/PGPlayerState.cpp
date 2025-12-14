#include "PlayerState/PGPlayerState.h"

APGPlayerState::APGPlayerState()
{
	
}

void APGPlayerState::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("[APGPlayerState] Name : %s, CharacterID : %d, TeamID : %d!"), *GetPlayerName(), CharacterID, TeamID);
}

void APGPlayerState::SetCharID(int32 NewCharID)
{
	this->CharacterID = NewCharID;
}

void APGPlayerState::SetTeamID(int32 NewTeamID)
{
	this->TeamID = NewTeamID;
}
