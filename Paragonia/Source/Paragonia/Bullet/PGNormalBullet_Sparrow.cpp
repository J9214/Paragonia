#include "Bullet/PGNormalBullet_Sparrow.h"

#include "TargetActor/PGRangedTargetActor.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

APGNormalBullet_Sparrow::APGNormalBullet_Sparrow()
{
	PrimaryActorTick.bCanEverTick = false;

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

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
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
	else if (!IsValid(EffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Not Valid Effect Class"));
		Destroy();
	}
	else if (!IsValid(Task))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Task is Not Valid"));
		Destroy();
	}
	else if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - Ability is Not Valid"));
		Destroy();
	}
	else
	{
		UAbilitySystemComponent* From = Owner->GetComponentByClass<UAbilitySystemComponent>();
		UAbilitySystemComponent* To = OtherActor->GetComponentByClass<UAbilitySystemComponent>();

		if (!IsValid(From))
		{
			UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - UAbilitySystemComponent is Not Valid"));
			Destroy();
		}
		else if (!IsValid(To))
		{
			Ability->CancelAbility(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), Ability->GetCurrentActivationInfo(), true);
			Destroy();
		}
		//if (!IsValid(From) || !IsValid(To))
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("APGNormalBullet_Sparrow::OnBeginOverlap - UAbilitySystemComponent is Not Valid"));
		//}
		else
		{
			//FGameplayEffectContextHandle EffectContextHandle = From->MakeEffectContext();
			//FGameplayEffectSpecHandle SpecHandle = From->MakeOutgoingSpec(EffectClass, 1.0f, EffectContextHandle);

			//From->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), To);

			AGameplayAbilityTargetActor* GenericActor = nullptr;
			if (Task->BeginSpawningActor(Ability, APGRangedTargetActor::StaticClass(), GenericActor))
			{
				if (IsValid(GenericActor))
				{
					APGRangedTargetActor* SpawnedActor = Cast<APGRangedTargetActor>(GenericActor);
					if (IsValid(SpawnedActor))
					{
						SpawnedActor->SourceActor = this;
						SpawnedActor->SetActorLocation(GetActorLocation());
						SpawnedActor->SetAttackData(AttackData);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Cast To APGRangedTargetActor Failed"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Generic Target Actor not Valid"));
				}

				Task->FinishSpawningActor(Ability, GenericActor);

				//UE_LOG(LogTemp, Warning, TEXT("%s Attack %s"), *From->GetAvatarActor()->GetFName().ToString(), *To->GetAvatarActor()->GetFName().ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Target Actor Spawn Failed"));
			}
		}
	}
}

void APGNormalBullet_Sparrow::InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData)
{
	Ability = InAbility;
	Task = InTask;
	AttackData = InAttackData;
}
