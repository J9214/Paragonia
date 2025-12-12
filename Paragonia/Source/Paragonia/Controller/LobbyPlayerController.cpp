// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PG_LobbyWidget.h"
#include "UI/PG_CharacterSelectWidget.h"

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

	// delegate 등록하기

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

        // CharSelectUIWidgetInstance->Init(); 
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
