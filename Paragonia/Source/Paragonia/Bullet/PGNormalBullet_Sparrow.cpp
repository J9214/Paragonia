#include "Bullet/PGNormalBullet_Sparrow.h"

#include "Bullet/PGMultiBulletCreator.h"
#include "GA/Sparrow/GA_SpawnBullet_Sparrow.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"

APGNormalBullet_Sparrow::APGNormalBullet_Sparrow()
	: APGCreateTargetActorBullet()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(SceneRoot);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Sphere);

	StaticMesh->SetRelativeLocation(FVector(-50.0f, 0.0f, 0.0f));
	StaticMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	Projectile->InitialSpeed = 2000.0f;
	Projectile->Velocity = FVector(1.0f, 0.0f, 0.2f);
	Projectile->bRotationFollowsVelocity = true;
}

void APGNormalBullet_Sparrow::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void APGNormalBullet_Sparrow::Destroyed()
{
	if (IsValid(CreatedBy))
	{
		UGA_SpawnBullet_Sparrow* SpawnBulletAbility = Cast<UGA_SpawnBullet_Sparrow>(Ability);
		if (IsValid(SpawnBulletAbility))
		{
			bool bIsEndAbility = !CreatedBy->LeftBulletCheckAndDestroy(this);
			if (bIsEndAbility)
			{
				SpawnBulletAbility->EndAbility(
					SpawnBulletAbility->GetCurrentAbilitySpecHandle(),
					SpawnBulletAbility->GetCurrentActorInfo(),
					SpawnBulletAbility->GetCurrentActivationInfo(),
					true, false);
			}
		}
	}

	Super::Destroyed();
}

void APGNormalBullet_Sparrow::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority()) return;

	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Not Valid Owner"));
		Destroy();
	}
	else if (!IsValid(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Not Valid OtherActor"));
		Destroy();
	}
	else if (Owner == OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Can't Hit Self"));
	}
	else if (Owner == OtherActor->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Same Owner"));
	}
	else if (!IsValid(Task))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Task is Not Valid"));
	}
	else if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Ability is Not Valid"));
	}
	else if (UAbilitySystemComponent* ASC = OtherActor->GetComponentByClass<UAbilitySystemComponent>())
	{
		if (IsValid(CreatedBy))
		{
			UGA_SpawnBullet_Sparrow* SpawnBulletAbility = Cast<UGA_SpawnBullet_Sparrow>(Ability);
			if (IsValid(SpawnBulletAbility))
			{
				bool bIsEndAbility = !CreatedBy->LeftBulletCheckAndDestroy(this);
				SpawnBulletAbility->SetIsEndAbility(bIsEndAbility);
			}
		}

		CreateTargetActor();
		if (!bIsPierce)
			Destroy();
	}
	else
	{
		Destroy();
	}
}
