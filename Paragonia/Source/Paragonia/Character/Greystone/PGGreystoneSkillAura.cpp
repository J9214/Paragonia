#include "Character/Greystone/PGGreystoneSkillAura.h"
#include "Engine/ReplicatedState.h"

APGGreystoneSkillAura::APGGreystoneSkillAura() :
	TickInterval(1.0f),
	LifeTime(5.0f),
	SourceActor(nullptr),
	SourceASC(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
}

void APGGreystoneSkillAura::InitFromAbility(AActor* InSourceActor, const FAttackData& InAttackData)
{
	SourceActor = InSourceActor;
	AttackData = InAttackData;
}

void APGGreystoneSkillAura::BeginPlay()
{
	Super::BeginPlay();
	
}

void APGGreystoneSkillAura::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

void APGGreystoneSkillAura::StartAura()
{
}

void APGGreystoneSkillAura::TickAura_SendHitCheck()
{
}

void APGGreystoneSkillAura::OnHitResultEvent(const FGameplayEventData Payload)
{
}