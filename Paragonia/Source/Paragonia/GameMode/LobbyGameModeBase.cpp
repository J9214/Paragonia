// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameModeBase.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"

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
}
