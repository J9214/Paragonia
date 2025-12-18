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
	else
	{
		++CurrentCount;

		HitCheckNotify();
		bool bIsEndAbility = (CurrentCount >= MaxCount);

		if (!bIsEndAbility)
		{
			SetSpawnTimer();
		}
		else
		{
			Destroy();
		}
	}
}