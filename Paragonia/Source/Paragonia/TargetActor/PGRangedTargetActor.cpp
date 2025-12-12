#include "TargetActor/PGRangedTargetActor.h"

#include "Bullet/PGNormalBullet_Sparrow.h"

#include "DrawDebugHelpers.h"

APGRangedTargetActor::APGRangedTargetActor()
{
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
}

void APGRangedTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
}

void APGRangedTargetActor::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TArray<FHitResult> OutHitResults;
		bool bHit = SphereTrace(OutHitResults);

		FGameplayAbilityTargetDataHandle DataHandle;
		if (bHit)
		{
			for (const FHitResult& Hit : OutHitResults)
			{
				DataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
			}
		}

		TargetDataReadyDelegate.Broadcast(DataHandle);

		APGNormalBullet_Sparrow* Bullet = Cast<APGNormalBullet_Sparrow>(SourceActor);
		if (IsValid(Bullet))
		{
			FColor DrawColor = bHit ? FColor::Green : FColor::Red;
			FVector TraceStart = SourceActor->GetActorLocation();
			FVector TraceEnd = SourceActor->GetActorLocation() + SourceActor->GetActorForwardVector() * AttackData.Range;
			FVector Center = TraceStart + (TraceEnd - TraceStart) * 0.5f;

			DrawDebugSphere(
				GetWorld(),
				Center,
				AttackData.Radius,
				16,
				DrawColor,
				false,
				5.0f
			);

			Bullet->Destroy();
		}
	}
}

void APGRangedTargetActor::SetAttackData(const FAttackData& InAttackData)
{
	AttackData = InAttackData;
}

bool APGRangedTargetActor::SphereTrace(TArray<FHitResult>& OutHitResults) const
{
	FVector Start = SourceActor->GetActorLocation();
	FVector End = Start + SourceActor->GetActorForwardVector() * AttackData.Range;

	FCollisionQueryParams Params(NAME_None, false, SourceActor);

	bool bHit = SourceActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackData.Radius),
		Params
	);

	return bHit;
}
