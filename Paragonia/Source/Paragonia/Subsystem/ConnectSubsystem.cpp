// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ConnectSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

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