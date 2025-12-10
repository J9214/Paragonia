// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ConnectSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

void UConnectSubsystem::ConnectToConfigIp()
{
	FString IpAddress;
	FString Port;

	// DefaultGame.ini 에서 읽어오기
	if (GConfig)
	{
#if WITH_EDITOR
		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("LocalServerIpAddress"),
			IpAddress,
			GGameIni
		);

		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("EditorPort"),
			Port,
			GGameIni
		);
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToConfigIp] Editor Port - [%s:%s]"), *IpAddress, *Port);
#else
		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("RealServerIpAddress"),
			IpAddress,
			GGameIni
		);

		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("ProductionPort"),
			Port,
			GGameIni
		);
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToConfigIp] Product Port - [%s:%s]"), *IpAddress, *Port);
#endif
	}

	if (IpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToConfigIp] : Config IP is Empty! Using Localhost."));
#if WITH_EDITOR
		IpAddress = TEXT("127.0.0.1");
#else
		IpAddress = TEXT("127.0.0.1");
#endif
	}

	if (Port.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToConfigIp] : Port IP is Empty! Using Defaults!"));
#if WITH_EDITOR
		Port = TEXT("17777");
#else
		Port = TEXT("7777");
#endif
	}

	FString FinalAddress = FString::Printf(TEXT("%s:%s"), *IpAddress, *Port);
	UE_LOG(LogTemp, Warning, TEXT("[ConnectToConfigIp] : %s"),*FinalAddress);
	ConnectToIpAddress(FinalAddress);
}

void UConnectSubsystem::ConnectToIpAddress(FString IpAddress)
{
	if (IpAddress.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToIpAddress] : IP Address is empty!"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (IsValid(PlayerController) &&
		PlayerController->IsLocalController() == true)
	{
		UE_LOG(LogTemp, Log, TEXT("[ConnectToIpAddress] : Traveling to Server: %s"), *IpAddress);

		PlayerController->ClientTravel(IpAddress, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConnectToIpAddress] : Player Controller Is Weird!"));
		return;
	}
}

void UConnectSubsystem::TravelToLobby()
{
	FString LevelPath;
	if (GConfig)
	{
		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("LobbyLevelPath"),
			LevelPath,
			GGameIni
		);
	}

	// 만약 설정이 없으면 하드코딩된 기본값 리턴 (안전장치)
	if (LevelPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TravelToLobby] : Config Lobby Levl is Empty!"));
		LevelPath = TEXT("/Game/Paragonia/Maps/Lobby");
	}

	ServerTravelToLevel(LevelPath);
}

void UConnectSubsystem::TravelToGame()
{
	FString LevelPath;
	if (GConfig)
	{
		GConfig->GetString(
			TEXT("/Script/Paragonia.ConnectSubsystem"),
			TEXT("GameLevelPath"),
			LevelPath,
			GGameIni
		);
	}

	// 만약 설정이 없으면 하드코딩된 기본값 리턴 (안전장치)
	if (LevelPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TravelToGame] : Config Game Levl is Empty!"));
		LevelPath = TEXT("/Game/Paragonia/Maps/Game");
	}

	ServerTravelToLevel(LevelPath);
}

void UConnectSubsystem::ServerTravelToLevel(FString LevelName)
{
	if (LevelName.IsEmpty() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ServerTravelToLevel] LevelName is empty."));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return;
	}

	if (World->GetNetMode() == NM_Client ||
		World->GetNetMode() == NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ServerTravelToLevel] Failed: Called By Client or Standalone."));
		return;
	}

	FString Url = FString::Printf(TEXT("/Game/Paragonia/Maps/%s?listen"), *LevelName);

	UE_LOG(LogTemp, Log, TEXT("[Server] ServerTravel to: %s"), *Url);

	World->ServerTravel(Url);
}