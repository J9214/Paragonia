#include "Controller/PGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GameState/PGGameStateBase.h"
#include "PlayerState/PGPlayerState.h"
#include "UI/UW_GameResult.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void APGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContexts, 0);
		}
	}
}

void APGPlayerController::BeginPlay()
{
	Super::BeginPlay();

    APGGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<APGGameStateBase>() : nullptr;
    if (GS)
    {
        // Delegate에 함수 바인딩 (동적 바인딩)
        GS->OnTeamResultChanged.AddDynamic(this, &APGPlayerController::OnTeamResultChanged);
    }



    // 팀 확인용 디버그 로그
    APGPlayerState* MyPS = Cast<APGPlayerState>(PlayerState);
    if (!MyPS)
        return;
    if (const UEnum* EnumPtr = StaticEnum<decltype(MyPS->TeamType)>())
    {
        FString EnumName = EnumPtr->GetNameStringByValue(static_cast<int64>(MyPS->TeamType));
    }
    

}

#pragma region GameResult
void APGPlayerController::OnTeamResultChanged(ETeamResult NewResult)
{
    APGGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<APGGameStateBase>() : nullptr;
    if (!GS)
        return;

    if (LastTeamResult != GS->TeamResult)
    {
        LastTeamResult = GS->TeamResult;

        // 자신의 PlayerState 팀 정보 얻기
        APGPlayerState* MyPS = Cast<APGPlayerState>(PlayerState);
        if (!MyPS)
            return;

        bool bIsWinner = false;
        switch (GS->TeamResult)
        {
        case ETeamResult::Team1Win:
            bIsWinner = (MyPS->TeamType == ETeamType::Team1);
            break;
        case ETeamResult::Team2Win:
            bIsWinner = (MyPS->TeamType == ETeamType::Team2);
            break;
        default:
            break;
        }

        // UI 업데이트 및 게임 종료 처리 예시
        if (bIsWinner)
        {
            ShowWinWidget(1);
        }
        else if (GS->TeamResult != ETeamResult::None)
        {
            ShowWinWidget(0);
        }
    }

    // 계속해서 팀 결과 감지를 위한 타이머를 설정하거나 OnRep에 연결 가능
}

void APGPlayerController::ShowWinWidget(uint8 IsWin)
{
    if (IsValid(GameResultUIClass) == true)
    {
        UUW_GameResult* GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
        if (IsValid(GameResultUI) == true)
        {
            GameResultUI->AddToViewport(0);

            FText GameResultText = (IsWin == 1)
                ? FText::FromString(TEXT("Victory"))
                : FText::FromString(TEXT("Defeat")); 
			GameResultUI->ResultText->SetText(GameResultText);

            FInputModeUIOnly Mode;
            if (GameResultUI->ReturnToTitleButton != nullptr)
            {
                Mode.SetWidgetToFocus(GameResultUI->ReturnToTitleButton->TakeWidget());
            }
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            SetInputMode(Mode);

            bShowMouseCursor = true;
        }
    }
}

#pragma endregion GameResult