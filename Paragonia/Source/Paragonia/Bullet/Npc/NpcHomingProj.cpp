// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet/Npc/NpcHomingProj.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Net/UnrealNetwork.h"

ANpcHomingProj::ANpcHomingProj()
	:MaxLifeTime(10.0f),
	TargetCheckTime(0.1f)
{
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ANpcHomingProj::OnOverlapBegin);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 0.0f;
	MovementComp->MaxSpeed = 0.0f;

	MovementComp->bIsHomingProjectile = true;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->HomingAccelerationMagnitude = 5000.0f;
	MovementComp->ProjectileGravityScale = 0.0f;
}

void ANpcHomingProj::InitializeProjectile(AActor* Target, float Speed)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (IsValid(Target) == false)
	{
		DeactivateProjectile();
		return;
	}

	bHitConfirmed = false;
	HomingTargetActor = Target;

	MovementComp->InitialSpeed = Speed;
	MovementComp->MaxSpeed = Speed;
	MovementComp->HomingTargetComponent = Target->GetRootComponent();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TargetCheckTimerHandle,
			this,
			&ANpcHomingProj::CheckTargetStatus,
			TargetCheckTime,
			true
		);
	}
}

void ANpcHomingProj::DeactivateProjectile()
{
	if (HasAuthority())
	{
		if (bHitConfirmed == false)
		{
			FGameplayEventData Payload;
			Payload.EventTag = HitEventTag;
			Payload.Instigator = GetInstigator();
			Payload.Target = nullptr;

			if (GetInstigator())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), HitEventTag, Payload);
			}
		}

		GetWorld()->GetTimerManager().ClearTimer(TargetCheckTimerHandle);
		Destroy();
	}
}

void ANpcHomingProj::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANpcHomingProj, HomingTargetActor);
}

void ANpcHomingProj::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetLifeSpan(MaxLifeTime);
	}
}

void ANpcHomingProj::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (OtherActor == this ||
		OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor != HomingTargetActor)
	{
		return;
	}

	bHitConfirmed = true;
	FGameplayEventData Payload;
	Payload.EventTag = HitEventTag;
	Payload.Instigator = GetInstigator();
	Payload.Target = OtherActor;

	FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	NewTargetData->HitResult = SweepResult;
	NewTargetData->HitResult.bBlockingHit = true;
	Payload.TargetData.Add(NewTargetData);

	if (AActor* MyOwner = GetInstigator())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MyOwner, HitEventTag, Payload);
	}

	// TODO : 이펙트 , 사운드? GC?

	DeactivateProjectile();
}

void ANpcHomingProj::OnRep_HomingTarget()
{
	if (IsValid(HomingTargetActor))
	{
		MovementComp->HomingTargetComponent = HomingTargetActor->GetRootComponent();
	}
}

void ANpcHomingProj::CheckTargetStatus()
{
	if (HasAuthority() == false)
	{
		return;
	}
	
	if (IsValid(HomingTargetActor) == false)
	{
		DeactivateProjectile();
		return;
	}

	// TODO : 타겟의 사망처리 중인 경우에 대한 처리

}
