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

	TeamType = ETeamType::Team1;
}

#pragma region Team

void APGPlayerState::OnRep_TeamType()
{
	// 팀 변경시 UI 갱신 등 처리
}

void APGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APGPlayerState, TeamType);
}

#pragma endregion Team
void APGPlayerState::SetCharID(int32 NewCharID)
{
	this->CharacterID = NewCharID;
}

void APGPlayerState::SetTeamID(int32 NewTeamID)
{
	this->TeamID = NewTeamID;
}
