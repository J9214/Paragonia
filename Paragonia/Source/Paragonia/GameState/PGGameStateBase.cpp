#include "GameState/PGGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/PGPlayerState.h"
#include "Controller/PGPlayerController.h"

APGGameStateBase::APGGameStateBase()
{
	bReplicates = true;
}
	
void APGGameStateBase::OnRep_TeamResult()
{
	OnTeamResultChanged.Broadcast(TeamResult);
}	

void APGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APGGameStateBase, TeamResult);
}
