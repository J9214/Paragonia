// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameModeBase.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "GameState/LobbyGameStateBase.h"
#include "PlayerState/LobbyPlayerState.h"
#include "Controller/LobbyPlayerController.h"
#include "Subsystem/ConnectSubsystem.h"
#include "Engine/GameInstance.h"

ALobbyGameModeBase::ALobbyGameModeBase()
	:PlayerCountToStart(2),
	CountdownDuration(5),
	NowReadyCount(0)
{
    GameStateClass = ALobbyGameStateBase::StaticClass();
    PlayerStateClass = ALobbyPlayerState::StaticClass();
    PlayerControllerClass = ALobbyPlayerController::StaticClass();
}

void ALobbyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Start Lobby!"));

	if (HasAuthority())
	{
        bool bCanBindAll = true;
        TSharedPtr<class FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);

        if (LocalIp.IsValid())
        {
            // false를 넣으면 포트 번호는 빼고 IP만 나옵니다.
            FString IPAddress = LocalIp->ToString(true);

            UE_LOG(LogTemp, Warning, TEXT("===================================="));
            UE_LOG(LogTemp, Warning, TEXT("   [SERVER INTERNAL IP]: %s"), *IPAddress);
            UE_LOG(LogTemp, Warning, TEXT("===================================="));
        }
	}

    ALobbyGameStateBase* LobbyGS = GetGameState<ALobbyGameStateBase>();
    if (IsValid(LobbyGS))
    {
        LobbyGS->SetLobbyState(EGameLobbyState::GLS_WaitingForPlayers);
        LobbyGS->SetLeftTime(CountdownDuration);
    }
}

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Log, TEXT("현재 접속자 수: %d"), GetNumPlayers());
}

void ALobbyGameModeBase::CheckAllPlayersReady()
{
	ALobbyGameStateBase* LobbyGS = GetGameState<ALobbyGameStateBase>();
	if (IsValid(LobbyGS) == false ||
		LobbyGS->GetCurrentLobbyState() != EGameLobbyState::GLS_CharacterSelect)
	{
		return;
	}

	bool bAllReady = true;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (IsValid(LobbyPS))
		{
			if (LobbyPS->GetPlayerLobbyState() != EPlayerLobbyState::PLS_SelectedAndReady)
			{
				bAllReady = false;
				break;
			}
		}
	}

	if (bAllReady)
	{
		UE_LOG(LogTemp, Log, TEXT("[CheckAllPlayersReady] Ready To Game!"));
		StartGameCountdown();
	}
}

void ALobbyGameModeBase::CheckStartingCondition()
{
	ALobbyGameStateBase* LobbyGS = GetGameState<ALobbyGameStateBase>();

	if (IsValid(LobbyGS) == false 
		|| LobbyGS->GetCurrentLobbyState() != EGameLobbyState::GLS_WaitingForPlayers)
	{
		return;
	}

	NowReadyCount = 0;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS && LobbyPS->GetPlayerLobbyState() == EPlayerLobbyState::PLS_MatchingReady)
		{
			NowReadyCount++;
		}
	}

	if (NowReadyCount >= PlayerCountToStart)
	{
		LobbyGS->SetLobbyState(EGameLobbyState::GLS_CharacterSelect);
	}
}

void ALobbyGameModeBase::StartGameCountdown()
{
	ALobbyGameStateBase* LobbyGS = GetGameState<ALobbyGameStateBase>();
	if (IsValid(LobbyGS))
	{
		LobbyGS->SetLobbyState(EGameLobbyState::GLS_GameStarting);
	}

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ALobbyGameModeBase::OnCountdownTimer, 1.0f, true);
}

void ALobbyGameModeBase::OnCountdownTimer()
{
	ALobbyGameStateBase* LobbyGS = GetGameState<ALobbyGameStateBase>();
	if (IsValid(LobbyGS))
	{
		int32 CurrentTime = LobbyGS->GetLeftTime();
		if (CurrentTime > 0)
		{
			LobbyGS->SetLeftTime(CurrentTime - 1);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
			ProcessServerTravel();
		}
	}
}

void ALobbyGameModeBase::ProcessServerTravel()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameModeBase] Game Instance is Not Valid...?"));
		return;
	}

	UConnectSubsystem* ConnectSubsystem = GameInstance->GetSubsystem<UConnectSubsystem>();
	if (IsValid(ConnectSubsystem))
	{
		bUseSeamlessTravel = true;
		UE_LOG(LogTemp, Log, TEXT("[LobbyGameModeBase] Travel to Game Level"));
		ConnectSubsystem->TravelToGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameModeBase] ConnectSubsystem is invalid!"));
	}
}
