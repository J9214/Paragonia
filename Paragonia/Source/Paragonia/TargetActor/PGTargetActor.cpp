#include "TargetActor/PGTargetActor.h"
#include "Abilities/GameplayAbility.h"
#include "DrawDebugHelpers.h"
#include "Character/PGPlayerCharacterBase.h"

APGTargetActor::APGTargetActor()
{
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
}

void APGTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void APGTargetActor::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TArray<FHitResult> OutHitResults;
		bool bHit = GetSphereTraceHitResult(SourceActor, OutHitResults);

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

bool APGTargetActor::GetSphereTraceHitResult(AActor* InSourceActor, TArray<FHitResult>& OutHitResults) const
{
	TSet<APGPlayerCharacterBase*> HitCharacters;
	FCollisionQueryParams Params(NAME_None, false, InSourceActor);

	const float AttackRange = 100.f;
	const float AttackRadius = 50.f;
	FVector ForwardVec = InSourceActor->GetActorForwardVector();
	FVector Start = InSourceActor->GetActorLocation();
	FVector End = Start + ForwardVec * AttackRange;
	bool bHit = InSourceActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(SourceActor);
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->DrawDebugAttackCollision(DrawColor, Start, End, ForwardVec);
	}

	return bHit;
}

