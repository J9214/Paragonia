#include "GA/Greystone/GA_SkillE_Greystone.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Struct/FAttackData.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "Character/Greystone/PGGreystoneSkillAura.h"

UGA_SkillE_Greystone::UGA_SkillE_Greystone()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_SkillE_Greystone::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Greystone::ActivateAbility - CommitAbility failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!IsValid(AttackData.Montage))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Greystone::ActivateAbility - Invalid AttackData Montage"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("SkillE_Task"),
			AttackData.Montage,
			1.0f
		);
	if (IsValid(Task))
	{
		Task->OnCompleted.AddDynamic(this, &UGA_SkillE_Greystone::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_SkillE_Greystone::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_SkillE_Greystone::OnMontageCancelled);

		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Greystone::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!HasAuthority(&ActivationInfo))
	{
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar) || !AuraClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APGGreystoneSkillAura* Aura = GetWorld()->SpawnActor<APGGreystoneSkillAura>(
		AuraClass,
		Avatar->GetActorLocation(),
		Avatar->GetActorRotation(),
		SpawnParams
	);

	if (IsValid(Aura))
	{
		Aura->InitFromAbility(Avatar, AttackData);
		Aura->AttachToActor(Avatar, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void UGA_SkillE_Greystone::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SkillE_Greystone::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillE_Greystone::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillE_Greystone::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}