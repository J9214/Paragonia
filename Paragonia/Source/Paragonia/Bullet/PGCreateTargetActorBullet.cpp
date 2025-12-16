#include "Bullet/PGCreateTargetActorBullet.h"

#include "TargetActor/PGRangedTargetActor.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

APGCreateTargetActorBullet::APGCreateTargetActorBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APGCreateTargetActorBullet::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void APGCreateTargetActorBullet::InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData)
{
	Ability = InAbility;
	Task = InTask;
	AttackData = InAttackData;
}

AGameplayAbilityTargetActor* APGCreateTargetActorBullet::CreateTargetActor()
{
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
				Destroy();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Generic Target Actor not Valid"));
			Destroy();
		}

		Task->FinishSpawningActor(Ability, GenericActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Actor Spawn Failed"));
		Destroy();
	}

	return GenericActor;
}
