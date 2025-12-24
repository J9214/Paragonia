#include "GA/LtBelica/GA_SkillQ_LtBelica.h"

UGA_SkillQ_LtBelica::UGA_SkillQ_LtBelica()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	TaskName = FName(TEXT("SkillQTask"));
	TargetingStartMontageSectionName = FName(TEXT("TargetingStart"));
	TargetingEndtMontageSectionName = FName(TEXT("TargetingEnd"));
}
