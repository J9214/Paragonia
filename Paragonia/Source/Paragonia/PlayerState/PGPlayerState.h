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

#pragma region Team
public:
	UFUNCTION()
	void OnRep_TeamType();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TeamType)
	ETeamType TeamType = ETeamType::None;

#pragma endregion Team

	void SetCharID(int32 NewCharID);
	void SetTeamID(int32 NewTeamID);

protected:
	int32 CharacterID;
	int32 TeamID;
};