#include "GA/Sparrow/GA_SkillQ_Sparrow.h"

UGA_SkillQ_Sparrow::UGA_SkillQ_Sparrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}
