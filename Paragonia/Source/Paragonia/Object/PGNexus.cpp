// Fill out your copyright notice in the Description page of Project Settings.


#include "PGNexus.h"
#include "GameMode/PGGameModeBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

APGNexus::APGNexus()
{
	Tags.Add(FName("Nexus"));
}

void APGNexus::BeginPlay()
{
	Super::BeginPlay();
}


void APGNexus::Destroyed()
{
    Super::Destroyed();

    OnNexusDestroyed.Broadcast(this);

    if (HasAuthority())
    {
        APGGameModeBase* GM = GetWorld() ? Cast<APGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())) : nullptr;
        if (GM)
        {
            GM->OnObjectiveDestroyed(this);
        }
    }
}