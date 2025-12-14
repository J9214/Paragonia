#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameState/PGGameStateBase.h"
#include "PGPlayerController.generated.h"

class UInputMappingContext;
class UUW_GameResult;

UCLASS()
class PARAGONIA_API APGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
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

protected:
	UFUNCTION()
	void OnTeamResultChanged(ETeamResult NewResult);

	UFUNCTION()
	void ShowWinWidget(uint8 IsWin);

private:
	// 이전 TeamResult를 저장하여 중복 처리 방지
	ETeamResult LastTeamResult = ETeamResult::None;

#pragma endregion GameResult
};
