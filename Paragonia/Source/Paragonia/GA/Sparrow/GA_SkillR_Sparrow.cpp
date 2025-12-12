#include "GA/Sparrow/GA_SkillR_Sparrow.h"

UGA_SkillR_Sparrow::UGA_SkillR_Sparrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}
