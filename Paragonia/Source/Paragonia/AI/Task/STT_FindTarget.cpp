
#include "AI/Task/STT_FindTarget.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/PGPlayerCharacterBase.h"

bool FSTT_FindTarget::Link(FStateTreeLinker& Linker)
{
    Linker.LinkExternalData(NpcActorHandle);
    return true;
}

EStateTreeRunStatus FSTT_FindTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    ANpcBaseCharacter* NPC = &Context.GetExternalData(NpcActorHandle);

    if (NPC == nullptr)
    {
        return EStateTreeRunStatus::Failed;
    }

    FVector SearchLocation = NPC->GetActorLocation();
    float Radius = NPC->GetSightRange();

    TArray<AActor*> OutActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(NPC);

    bool bFound = UKismetSystemLibrary::SphereOverlapActors(
        NPC,
        SearchLocation,
        Radius,
        ObjectTypes,
        nullptr,
        ActorsToIgnore,
        OutActors
    );

    if (bFound == false)
    {
        return EStateTreeRunStatus::Failed;
    }

    AActor* BestTarget = nullptr;
    float MinDistanceSq = FLT_MAX;

    for (AActor* TargetActor : OutActors)
    {
        if (IsValid(TargetActor) == false) 
            continue;

        // TeamId 관련 별도의 Interface 함수 제작 하는것이 깔끔해보임
        if (ANpcBaseCharacter* PotentialEnemy = Cast<ANpcBaseCharacter>(TargetActor))
        {
            if (PotentialEnemy->GetTeamId() == NPC->GetTeamId())
                continue;
        }
        /*else if(APGPlayerCharacterBase* PgCharacter = Cast<APGPlayerCharacterBase>(TargetActor))
        {
            if (PgCharacter->GetTeamId() == NPC->GetTeamId())
                continue;
        }*/

        float DistSq = FVector::DistSquared(NPC->GetActorLocation(), TargetActor->GetActorLocation());

        if (DistSq < MinDistanceSq)
        {
            MinDistanceSq = DistSq;
            BestTarget = TargetActor;
        }
    }

    if (BestTarget)
    {
        NPC->SetAttackTarget(BestTarget);
        return EStateTreeRunStatus::Succeeded;
    }

    return EStateTreeRunStatus::Failed;
}