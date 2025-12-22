#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);

class UPGInventoryComponent;
class UPGInventoryWidget;

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

public:
	UPROPERTY(VisibleAnywhere, Replicated)
	bool bClientReady = false;


#pragma region Shop
public:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Gold, BlueprintReadOnly)
	int32 Gold = 1000;

	UPROPERTY(BlueprintAssignable)
	FOnGoldChanged OnGoldChanged;

	UFUNCTION()
	void OnRep_Gold();

	bool CanAfford(int32 Cost) const { return Gold >= Cost; }
	void AddGold(int32 Delta);
#pragma endregion Shop

#pragma region Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPGInventoryComponent> Inventory;

#pragma endregion Inventory
};