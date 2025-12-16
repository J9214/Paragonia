// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PG_LobbyPreviewCharacter.h"

// Sets default values
APG_LobbyPreviewCharacter::APG_LobbyPreviewCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MeshComp->SetSimulatePhysics(false);
    MeshComp->SetEnableGravity(false);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APG_LobbyPreviewCharacter::SetCharacterMesh(USkeletalMesh* NewMesh, UClass* AnimInstance)
{
    if (NewMesh)
    {
        MeshComp->SetSkeletalMesh(NewMesh);

        MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        MeshComp->SetAnimInstanceClass(AnimInstance);
    }
}

void APG_LobbyPreviewCharacter::PlayMontage(UAnimMontage* Montage)
{
    if (!Montage) return;

    if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
    {
        AnimInst->Montage_Play(Montage);
    }
}
