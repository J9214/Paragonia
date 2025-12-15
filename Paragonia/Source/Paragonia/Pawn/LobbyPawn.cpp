// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/LobbyPawn.h"
#include <Controller/LobbyPlayerController.h>
#include "UI/PG_CharacterSelectWidget.h"

ALobbyPawn::ALobbyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyPawn::BeginPlay()
{
	Super::BeginPlay();

    SetActorRotation(StartRotation);
}

void ALobbyPawn::StartMoveToTarget()
{
	bIsMovingToTarget = true;
}


void ALobbyPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bIsMovingToTarget)
        return;

    const FVector Current = GetActorLocation();
    FVector ToTarget = TargetLocation - Current;
    const float Dist = ToTarget.Length();

    if (Dist <= ArriveTolerance)
    {
        SetActorLocation(TargetLocation);
        bIsMovingToTarget = false;

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ALobbyPlayerController* PGPC = Cast<ALobbyPlayerController>(PC))
            {
                UPG_CharacterSelectWidget* CharacterSelectWidget = PGPC->GetCharSelectUIInst();
                if (CharacterSelectWidget)
                    CharacterSelectWidget->SetUI();
            }
        }

        return;
    }

    ToTarget.Normalize();
    const FVector NewLoc = Current + ToTarget * MoveSpeed * DeltaSeconds;
    SetActorLocation(NewLoc);

    const FRotator CurrentRot = GetActorRotation();

    FRotator NewRot = FMath::RInterpTo(
        CurrentRot,
        TargetRotation,
        DeltaSeconds,
        RotationInterpSpeed
    );

    SetActorRotation(NewRot);
}
