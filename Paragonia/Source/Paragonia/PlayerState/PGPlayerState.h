#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PGPlayerState.generated.h"

UCLASS()
class PARAGONIA_API APGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APGPlayerState();

	void SetCharID(int32 NewCharID);
	void SetTeamID(int32 NewTeamID);

protected:
	int32 CharacterID;
	int32 TeamID;
};