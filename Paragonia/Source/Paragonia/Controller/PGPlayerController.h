#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameState/PGGameStateBase.h"
#include "PGPlayerController.generated.h"

class UInputMappingContext;
class UUW_GameResult;
class UPG_IngameHUD;

UCLASS()
class PARAGONIA_API APGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Client, Reliable)
	void Client_SetExpectedPlayerCount(int32 InExpectedPlayerCount);

	UFUNCTION(Client, Reliable)
	void Client_AllClientsReady();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ReportClientReady();

protected:
	void StartReadyCheck();
	void TickReadyCheck();
	bool AreAllPlayersReplicatedOnThisClient() const;

	FTimerHandle ReadyCheckTimerHandle;
	float ReadyCheckIntervalSeconds = 0.1f;

	int32 ExpectedPlayerCount = 0;
	bool bLocalReadyReported = false;
protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContexts;

#pragma region GameResult
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUW_GameResult> GameResultUIClass;

	UPROPERTY()
	UUW_GameResult* GameResultUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPG_IngameHUD> IngameHUDClass;

	UPROPERTY()
	UPG_IngameHUD* IngameHUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> LoadingHUDClass;

	UPROPERTY()
	UUserWidget* LoadingHUD;
protected:
	UFUNCTION()
	void OnTeamResultChanged(ETeamResult NewResult);

	UFUNCTION()
	void ShowWinWidget(uint8 IsWin);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowLoadingHUD();

private:
	// 이전 TeamResult를 저장하여 중복 처리 방지
	ETeamResult LastTeamResult = ETeamResult::None;

#pragma endregion GameResult
};
