// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PG_LobbyWidget.h"
#include "UI/PG_CharacterSelectWidget.h"
#include "GameState/LobbyGameStateBase.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

    if (MatchStartUIWidgetClass)
    {
        MatchStartUIWidgetInstance = CreateWidget<UPG_LobbyWidget>(this, MatchStartUIWidgetClass);
        if (MatchStartUIWidgetInstance)
        {
            MatchStartUIWidgetInstance->AddToViewport();
            MatchStartUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (CharSelectUIWidgetClass)
    {
        CharSelectUIWidgetInstance = CreateWidget<UPG_CharacterSelectWidget>(this, CharSelectUIWidgetClass);
        if (CharSelectUIWidgetInstance)
        {
            CharSelectUIWidgetInstance->AddToViewport();
            CharSelectUIWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    bShowMouseCursor = true;
    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);

    SetupLobbyUI();
}

void ALobbyPlayerController::ShowCharacterSelect()
{
    if (MatchStartUIWidgetInstance)
    {
        MatchStartUIWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (CharSelectUIWidgetInstance)
    {
        CharSelectUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);

        CharSelectUIWidgetInstance->SetInit(); 
    }
}

void ALobbyPlayerController::ShowMatchStart()
{
    if (CharSelectUIWidgetInstance)
    {
        CharSelectUIWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (MatchStartUIWidgetInstance)
    {
        MatchStartUIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }
}

void ALobbyPlayerController::SetupLobbyUI()
{
    ALobbyGameStateBase* LobbyGS = GetWorld()->GetGameState<ALobbyGameStateBase>();

    if (IsValid(LobbyGS))
    {
        LobbyGS->OnLobbyGameStateChanged.AddDynamic(this, &ALobbyPlayerController::HandleLobbyStateChanged);

        HandleLobbyStateChanged(LobbyGS->GetCurrentLobbyState());
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ALobbyPlayerController::SetupLobbyUI);
    }
}

void ALobbyPlayerController::HandleLobbyStateChanged(EGameLobbyState NewState)
{
    switch (NewState)
    {
    case EGameLobbyState::GLS_WaitingForPlayers:
        ShowMatchStart();
        break;
    case EGameLobbyState::GLS_CharacterSelect:
        ShowCharacterSelect();
        break;
    case EGameLobbyState::GLS_GameStarting:
        break;
    default:
        break;
    }
}
