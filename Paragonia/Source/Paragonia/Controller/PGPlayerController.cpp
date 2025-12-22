#include "Controller/PGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GameState/PGGameStateBase.h"
#include "PlayerState/PGPlayerState.h"
#include "UI/UW_GameResult.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include <GameMode/PGGameModeBase.h>
#include "Character/PGPlayerCharacterBase.h"
#include "UI/HUDs/PG_IngameHUD.h"
#include "AttributeSet/CharacterAttributeSet.h"

void APGPlayerController::Client_SetExpectedPlayerCount_Implementation(int32 InExpectedPlayerCount)
{
    ExpectedPlayerCount = InExpectedPlayerCount;
}

void APGPlayerController::Client_AllClientsReady_Implementation()
{
    GetWorldTimerManager().ClearTimer(ReadyCheckTimerHandle);

    ShowGameHUD();
    BindIngameHUD();
}

void APGPlayerController::ServerRPC_ReportClientReady_Implementation()
{
    if (APGPlayerState* PS = GetPlayerState<APGPlayerState>())
    {
        PS->bClientReady = true;
    }

    if (APGGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<APGGameModeBase>() : nullptr)
    {
        GM->CheckAllClientsReady();
    }
}

void APGPlayerController::StartReadyCheck()
{
    if (!GetWorld())
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(ReadyCheckTimerHandle);
    GetWorldTimerManager().SetTimer(
        ReadyCheckTimerHandle,
        this,
        &ThisClass::TickReadyCheck,
        ReadyCheckIntervalSeconds,
        true
    );
}

void APGPlayerController::TickReadyCheck()
{
    if (bLocalReadyReported)
    {
        GetWorldTimerManager().ClearTimer(ReadyCheckTimerHandle);
        return;
    }

    if (!AreAllPlayersReplicatedOnThisClient())
    {
        return;
    }

    bLocalReadyReported = true;
    ServerRPC_ReportClientReady();
    GetWorldTimerManager().ClearTimer(ReadyCheckTimerHandle);
}

bool APGPlayerController::AreAllPlayersReplicatedOnThisClient() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    if (ExpectedPlayerCount <= 0)
    {
        return false;
    }

    AGameStateBase* GS = World->GetGameState();
    if (!GS)
    {
        return false;
    }

    APGPlayerState* LocalPS = GetPlayerState<APGPlayerState>();
    if (!IsValid(LocalPS))
    {
        return false;
    }

    if (!IsValid(GetPawn()))
    {
        return false;
    }

    const TArray<APlayerState*>& PlayerArray = GS->PlayerArray;
    if (PlayerArray.Num() < ExpectedPlayerCount)
    {
        return false;
    }

    for (APlayerState* PS : PlayerArray)
    {
        if (!IsValid(PS))
        {
            return false;
        }

        APawn* FoundPawn = PS->GetPawn();
        if (!IsValid(FoundPawn))
        {
            return false;
        }

        if (!FoundPawn->HasActorBegunPlay())
        {
            return false;
        }
    }

    return true;
}

bool APGPlayerController::BindIngameHUD()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    AGameStateBase* GS = World->GetGameState();
    if (!GS)
    {
        return false;
    }

    const TArray<APlayerState*>& PlayerArray = GS->PlayerArray;

    if (PlayerArray.Num() < ExpectedPlayerCount)
    {
        return false;
    }

    APGPlayerState* LocalPS = GetPlayerState<APGPlayerState>();
    if (!IsValid(LocalPS))
    {
        return false;
    }

    if (!SetMyHPBar(LocalPS))
    {
        return false;
    }

    if (!SetTeamHPBar(PlayerArray, LocalPS))
    {
        return false;
    }

    return true;
}

bool APGPlayerController::SetMyHPBar(APGPlayerState* LocalPS)
{
    APGPlayerCharacterBase* FoundMyCharacter = LocalPS->GetPawn<APGPlayerCharacterBase>();
    if (!IsValid(FoundMyCharacter))
    {
        return false;
    }

    if (!FoundMyCharacter->HasActorBegunPlay())
    {
        return false;
    }

    UCharacterAttributeSet* MyAttributeSet = FoundMyCharacter->GetCharacterAttributeSet();

    if (!IsValid(MyAttributeSet))
    {
        return false;
    }

    MyAttributeSet->OnHealthChanged_UI.RemoveAll(IngameHUD);
    MyAttributeSet->OnMaxHealthChanged_UI.RemoveAll(IngameHUD);

    MyAttributeSet->OnHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandlePlayerHealthChanged);
    MyAttributeSet->OnMaxHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandlePlayerMaxHealthChanged);

    IngameHUD->HandlePlayerMaxHealthChanged(MyAttributeSet->GetMaxHealth(), MyAttributeSet->GetMaxHealth());
    IngameHUD->HandlePlayerHealthChanged(MyAttributeSet->GetHealth(), MyAttributeSet->GetHealth());

    IngameHUD->InitMinimap(FoundMyCharacter->GetMinimapRenderTarget());

    return true;
}

bool APGPlayerController::SetTeamHPBar(const TArray<APlayerState*>& PlayerArray, APGPlayerState* LocalPS)
{
    int index = 0;

    for (APlayerState* PS : PlayerArray)
    {
        if (!IsValid(PS))
        {
            return false;
        }

        APGPlayerState* PGPS = Cast<APGPlayerState>(PS);

        if (PGPS == LocalPS || PGPS->GetTeamID() != LocalPS->GetTeamID())
        {
            continue;
        }

        APGPlayerCharacterBase* FoundCharacter = PS->GetPawn<APGPlayerCharacterBase>();
        if (!IsValid(FoundCharacter))
        {
            return false;
        }

        if (!FoundCharacter->HasActorBegunPlay())
        {
            return false;
        }

        UCharacterAttributeSet* TeamAttributeSet = FoundCharacter->GetCharacterAttributeSet();

        if (!IsValid(TeamAttributeSet))
        {
            return false;
        }

        TeamAttributeSet->OnHealthChanged_UI.RemoveAll(IngameHUD);
        TeamAttributeSet->OnMaxHealthChanged_UI.RemoveAll(IngameHUD);

        switch (index)
        {
        case 0:

            IngameHUD->InitTeam1IngameIcon(PGPS->GetCharID());

            TeamAttributeSet->OnHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandleTeam1HealthChanged);
            TeamAttributeSet->OnMaxHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandleTeam1MaxHealthChanged);

            IngameHUD->HandleTeam1MaxHealthChanged(TeamAttributeSet->GetMaxHealth(), TeamAttributeSet->GetMaxHealth());
            IngameHUD->HandleTeam1HealthChanged(TeamAttributeSet->GetHealth(), TeamAttributeSet->GetHealth());

            break;
        default:

            IngameHUD->InitTeam2IngameIcon(PGPS->GetCharID());

            TeamAttributeSet->OnHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandleTeam2HealthChanged);
            TeamAttributeSet->OnMaxHealthChanged_UI.AddDynamic(IngameHUD, &UPG_IngameHUD::HandleTeam2MaxHealthChanged);

            IngameHUD->HandleTeam2MaxHealthChanged(TeamAttributeSet->GetMaxHealth(), TeamAttributeSet->GetMaxHealth());
            IngameHUD->HandleTeam2HealthChanged(TeamAttributeSet->GetHealth(), TeamAttributeSet->GetHealth());
            break;
        }

        index++;
    }

    return true;
}

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

    if (IsLocalPlayerController())
    {
        StartReadyCheck();
        ShowLoadingHUD();
    }
}

#pragma region GameResult
void APGPlayerController::OnTeamResultChanged(ETeamResult NewResult)
{
    APGGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<APGGameStateBase>() : nullptr;
    if (!GS)
    {
        return;
    }

    if (LastTeamResult != GS->TeamResult)
    {
        LastTeamResult = GS->TeamResult;

        // 자신의 PlayerState 팀 정보 얻기
        APGPlayerState* MyPS = Cast<APGPlayerState>(PlayerState);
        if (!MyPS)
        {
            return;
        }

        bool bIsWinner = false;
        switch (GS->TeamResult)
        {
        case ETeamResult::Team1Win:
            bIsWinner = (MyPS->GetTeamID() == 0);
            break;
        case ETeamResult::Team2Win:
            bIsWinner = (MyPS->GetTeamID() == 1);
            break;
        default:
            break;
        }

        // UI 업데이트 및 게임 종료 처리 예시
        if (bIsWinner)
        {
            ShowWinWidget(1);
        }
        else if (!bIsWinner)
        {
            ShowWinWidget(0);
        }
    }

    // 계속해서 팀 결과 감지를 위한 타이머를 설정하거나 OnRep에 연결 가능
}

void APGPlayerController::ShowWinWidget(uint8 IsWin)
{
    if (LoadingHUD)
    {
        LoadingHUD->RemoveFromParent();
        LoadingHUD = nullptr;
    }

    if (IngameHUD)
    {
        IngameHUD->RemoveFromParent();
        IngameHUD = nullptr;
    }

    if (IsValid(GameResultUIClass) == true)
    {
        GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
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

void APGPlayerController::ShowGameHUD()
{
    if (LoadingHUD)
    {
        LoadingHUD->RemoveFromParent();
        LoadingHUD = nullptr;
    }

    if (GameResultUI)
    {
        GameResultUI->RemoveFromParent();
        GameResultUI = nullptr;
    }

    if (IngameHUDClass)
    {
        IngameHUD = CreateWidget<UPG_IngameHUD>(this, IngameHUDClass);
        if (IngameHUD)
        {
            IngameHUD->AddToViewport();
            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
        }
    }
}

void APGPlayerController::ShowLoadingHUD()
{
    if (GameResultUI)
    {
        GameResultUI->RemoveFromParent();
        GameResultUI = nullptr;
    }

    if (IngameHUD)
    {
        IngameHUD->RemoveFromParent();
        IngameHUD = nullptr;
    }

    if (LoadingHUDClass)
    {
        LoadingHUD = CreateWidget<UUserWidget>(this, LoadingHUDClass);
        if (LoadingHUD)
        {
            LoadingHUD->AddToViewport();
            bShowMouseCursor = false;
            SetInputMode(FInputModeUIOnly());
        }
    }
}

#pragma endregion GameResult