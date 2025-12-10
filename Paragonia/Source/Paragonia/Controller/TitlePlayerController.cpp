// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Subsystem/ConnectSubsystem.h"
#include "Engine/GameInstance.h"

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(UIWidgetClass) == true)
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (IsValid(UIWidgetInstance) == true)
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

	ConnectLobby();
}

void ATitlePlayerController::ConnectLobby()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitlePC] Game Instance is Not Valid...?"));
		return;
	}

	UConnectSubsystem* ConnectSubsystem = GameInstance->GetSubsystem<UConnectSubsystem>();
	if (IsValid(ConnectSubsystem))
	{
		UE_LOG(LogTemp, Log, TEXT("[TitlePC] Requesting connection to Lobby..."));

		ConnectSubsystem->ConnectToConfigIp();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitlePC] ConnectSubsystem is invalid!"));
	}
}
