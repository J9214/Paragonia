#include "PlayerState/PGPlayerState.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Character/PGPlayerCharacterBase.h"

APGPlayerState::APGPlayerState()
{
}

void APGPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void APGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APGPlayerState, TeamID);
	DOREPLIFETIME(APGPlayerState, CharacterID);
}

void APGPlayerState::SetCharID(int32 NewCharID)
{
	this->CharacterID = NewCharID;
}

void APGPlayerState::SetTeamID(int32 NewTeamID)
{
	this->TeamID = NewTeamID;
}
