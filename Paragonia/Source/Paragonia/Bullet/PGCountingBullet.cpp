#include "Bullet/PGCountingBullet.h"

#include "GA/Sparrow/GA_SpawnBullet_Sparrow.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

void APGCountingBullet::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetSpawnTimer();
	}
}

void APGCountingBullet::SetSpawnTimer()
{
	GetWorldTimerManager().SetTimer(
		SpawnActorTimer,
		this,
		&ThisClass::OnCreateTargetActor,
		Interval,
		false
	);
}

void APGCountingBullet::OnCreateTargetActor()
{
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGCountingBullet::OnCreateTargetActor - Not Valid Owner"));
		Destroy();
	}
	else if (!IsValid(EffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGCountingBullet::OnCreateTargetActor - Not Valid Effect Class"));
		Destroy();
	}
	else if (!IsValid(Task))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGCountingBullet::OnCreateTargetActor - Task is Not Valid"));
		Destroy();
	}
	else if (!IsValid(Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGCountingBullet::OnCreateTargetActor - Ability is Not Valid"));
		Destroy();
	}
	else
	{
		++CurrentCount;

		bool bIsEndAbility = (CurrentCount >= MaxCount);
		UGA_SpawnBullet_Sparrow* SpawnActorAbility = Cast<UGA_SpawnBullet_Sparrow>(Ability);
		if (IsValid(SpawnActorAbility))
		{
			SpawnActorAbility->SetIsEndAbility(bIsEndAbility);
		}

		if (!IsValid(TargetActor))
		{
			TargetActor = CreateTargetActor();
		}

		if (IsValid(TargetActor))
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
		OnEffectCreate();

		if (!bIsEndAbility)
		{
			SetSpawnTimer();
		}
		else
		{
			TargetActor->Destroy();
		}
	}
}