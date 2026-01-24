#include "TargetActor/PGRangedTargetActor.h"

#include "Bullet/PGNormalBullet_Sparrow.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include "AbilitySystemComponent.h"

#include "DrawDebugHelpers.h"

APGRangedTargetActor::APGRangedTargetActor()
{
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
		bool bHit = PerformTrace(OutHitResults);

		FGameplayAbilityTargetDataHandle DataHandle;
		if (bHit)
		{
			for (const FHitResult& Hit : OutHitResults)
			{
				DataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
			}
		}

		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void APGRangedTargetActor::SetAttackData(const FAttackData& InAttackData)
{
	AttackData = InAttackData;
}

bool APGRangedTargetActor::PerformTrace(TArray<FHitResult>& OutHits) const
{
	switch (AttackData.SweepShape)
	{
	case EPGAttackShape::Sphere:
		return SphereTrace(OutHits);

	case EPGAttackShape::Box:
		return BoxTrace(OutHits);

	default:
		return false;
	}
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

	for (int i = 0; i < OutHitResults.Num(); )
	{
		FHitResult Result = OutHitResults[i];
		UAbilitySystemComponent* ASC = Result.GetActor()->GetComponentByClass<UAbilitySystemComponent>();
		if (!IsValid(ASC))
		{
			OutHitResults.RemoveAt(i);
		}
		else if (OutHitResults[i].GetActor() == SourceActor->Owner)
		{
			OutHitResults.RemoveAt(i);
		}
		else
		{
			++i;
		}
	}

	APGCreateTargetActorBullet* Bullet = Cast<APGCreateTargetActorBullet>(SourceActor);
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
	}

	return bHit;
}

bool APGRangedTargetActor::BoxTrace(TArray<FHitResult>& OutHits) const
{
	FVector Start = SourceActor->GetActorLocation();
	FVector End = Start + SourceActor->GetActorForwardVector() * AttackData.Range;

	FCollisionQueryParams Params(NAME_None, false, SourceActor);

	ACharacter* Character = Cast<ACharacter>(SourceActor);
	float HalfHeight = 88.f;

	if (Character)
	{
		UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
		if (Capsule)
		{
			HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		}
	}

	FVector BoxExtent;
	BoxExtent.X = AttackData.Range * 0.5f;
	BoxExtent.Y = AttackData.Radius;
	BoxExtent.Z = HalfHeight;

	bool bHit = SourceActor->GetWorld()->SweepMultiByChannel(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);



	APGCreateTargetActorBullet* Bullet = Cast<APGCreateTargetActorBullet>(SourceActor);
	if (IsValid(Bullet))
	{
		FColor DrawColor = bHit ? FColor::Green : FColor::Red;
		FVector Center = Start + (End - Start) * 0.5f;
		DrawDebugBox(
			GetWorld(),
			Center,
			BoxExtent,
			GetActorRotation().Quaternion(),
			DrawColor,
			false,
			3.0f
		);
	}

	return bHit;
}
