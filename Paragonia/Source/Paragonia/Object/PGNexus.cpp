// Fill out your copyright notice in the Description page of Project Settings.


#include "PGNexus.h"
#include "GameMode/PGGameModeBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

APGNexus::APGNexus()
{
	Tags.Add(FName("Nexus"));
    //Tags.Add(FName("Team2Nexus"));
    

    OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
    OverlapSphere->InitSphereRadius(200.0f);
    OverlapSphere->SetCollisionProfileName(TEXT("Trigger"));
    OverlapSphere->SetupAttachment(RootComponent);

    // BeginOverlap에 함수 바인딩
    OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APGNexus::OnOverlapBegin);
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

	this->Destroy();
}

void APGNexus::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // 여기서 플레이어나 원하는 액터인지 체크 가능
    Destroyed();
}

