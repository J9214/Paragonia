#include "PlayerState/PGPlayerState.h"

APGPlayerState::APGPlayerState()
{
	
}

void APGPlayerState::SetCharID(int32 NewCharID)
{
	this->CharacterID = NewCharID;
}

void APGPlayerState::SetTeamID(int32 NewTeamID)
{
	this->TeamID = NewTeamID;
}
