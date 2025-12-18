#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PGPlayerState.generated.h"

UENUM(BlueprintType)
enum class ETeamType : uint8 { None, Team1, Team2 };

UCLASS()
class PARAGONIA_API APGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APGPlayerState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	int32 GetCharID() const { return CharacterID; }
	int32 GetTeamID() const { return TeamID; }
	void SetCharID(int32 NewCharID);
	void SetTeamID(int32 NewTeamID);

protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	int32 CharacterID;
	UPROPERTY(VisibleAnywhere, Replicated)
	int32 TeamID;
};