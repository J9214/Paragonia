
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
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(NPC);

	bool bFoundAny = UKismetSystemLibrary::SphereOverlapActors(
		NPC, MyLoc, Radius, ObjectTypes, nullptr, IgnoreActors, OutActors
	);

	if (bFoundAny == false)
	{
		return EStateTreeRunStatus::Running;
	}

	AActor* BestTarget = nullptr;
	float MinDistSq = FLT_MAX;
	int32 MyTeamId = 255;

	if (NPC->GetClass()->ImplementsInterface(UPGTeamStatusInterface::StaticClass()))
	{
		MyTeamId = IPGTeamStatusInterface::Execute_GetTeamID(NPC);
	}

	for (AActor* Target : OutActors)
	{
		if (IsValid(Target) == false)
		{
			continue;
		}

		if (Target->Implements<UPGTeamStatusInterface>() == false)
		{
			continue;
		}

		int32 TargetTeamId = IPGTeamStatusInterface::Execute_GetTeamID(Target);

		if (TargetTeamId == MyTeamId)
		{
			continue;
		}

		if (IPGTeamStatusInterface::Execute_GetIsDead(Target) == true)
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
