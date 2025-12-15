#include "GA/PGSelfBuffAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UPGSelfBuffAbility::UPGSelfBuffAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UPGSelfBuffAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ApplyAttackDataOwnerEffects_OnActivate(AttackData);

	UAbilityTask_WaitDelay* DelayTask =
		UAbilityTask_WaitDelay::WaitDelay(this, Duration);

	DelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFinishDelay);
}

void UPGSelfBuffAbility::OnFinishDelay()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
