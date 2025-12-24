
#include "AI/Task/STT_FindTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/PGPlayerCharacterBase.h"
#include "AbilitySystemComponent.h" 
#include "AbilitySystemInterface.h"

bool FSTT_FindTarget::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(NpcActorHandle);
	return true;
}

EStateTreeRunStatus FSTT_FindTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.CurrentTimer = 0.0f;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_FindTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

	if (IsValid(NPC) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.CurrentTimer -= DeltaTime;
	if (InstanceData.CurrentTimer > 0.0f)
	{
		return EStateTreeRunStatus::Running;
	}

	InstanceData.CurrentTimer = InstanceData.ScanInterval;

	FVector MyLoc = NPC->GetActorLocation();
	float Radius = NPC->GetSightRange();

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(NPC);

	bool bFoundAny = UKismetSystemLibrary::SphereOverlapActors(
		NPC, MyLoc, Radius, ObjectTypes, nullptr, IgnoreActors, OutActors
	);

	// 탐색 결과 없음
	if (bFoundAny == false)
	{
		return EStateTreeRunStatus::Running;
	}

	AActor* BestTarget = nullptr;
	float MinDistSq = FLT_MAX;
	int32 MyTeamId = NPC->GetTeamId();

	for (AActor* Target : OutActors)
	{
		ANpcBaseCharacter* Enemy = Cast<ANpcBaseCharacter>(Target);
		if (IsValid(Enemy) == false ||
			Enemy->GetTeamId() == MyTeamId)
		{
			continue;
		}

		UAbilitySystemComponent* EnemyASC = Enemy->GetAbilitySystemComponent();
		if (IsValid(EnemyASC) == false ||
			EnemyASC->HasMatchingGameplayTag(Enemy->GetDeadTag()) == true)
		{
			continue;
		}

		float DistSq = FVector::DistSquared(MyLoc, Target->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			BestTarget = Target;
		}
	}

	if (BestTarget)
	{
		NPC->SetAttackTarget(BestTarget);

		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
