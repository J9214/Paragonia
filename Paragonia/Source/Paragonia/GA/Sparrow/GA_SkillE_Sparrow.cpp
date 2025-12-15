#include "GA/Sparrow/GA_SkillE_Sparrow.h"

UGA_SkillE_Sparrow::UGA_SkillE_Sparrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	TaskName = FName(TEXT("SkillETask"));
	TargetingStartMontageSectionName = FName(TEXT("ChargeStart"));
	TargetingEndtMontageSectionName = FName(TEXT("ChargeEnd"));
}
