#include "GA/Aurora/GA_SkillE_Aurora.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Struct/FAttackData.h"
#include "Animation/PGAnimInstance.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/PGPlayerCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/PGGameplayTags.h"

UGA_SkillE_Aurora::UGA_SkillE_Aurora()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_SkillE_Aurora::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character))
	{
		if (UAnimInstance* AI = Character->GetMesh()->GetAnimInstance())
		{
			if (auto PGAnimInstance = Cast<UPGAnimInstance>(AI))
			{
				PGAnimInstance->SetCurrentAttackData(AttackData);
			}
		}
	}

	if (!IsValid(AttackData.Montage))
	{
		return;
	}

	if (HasAuthority(&ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* HitResultTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Character.HitResult"));

		if (IsValid(HitResultTask))
		{
			HitResultTask->EventReceived.AddDynamic(this, &UGA_SkillE_Aurora::OnHitResultEvent);
			HitResultTask->ReadyForActivation();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::ActivateAbility - Failed to create HitResult Ability Task"));
		}
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("AttackTask"),
			AttackData.Montage,
			1.0f
		);

	if (IsValid(Task))
	{
		Task->OnCompleted.AddDynamic(this, &UGA_SkillE_Aurora::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_SkillE_Aurora::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &UGA_SkillE_Aurora::OnMontageCancelled);

		Task->ReadyForActivation();

		if (IsValid(Character))
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::ActivateAbility - Failed to create Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_SkillE_Aurora::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Character))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SkillE_Aurora::OnHitResultEvent(const FGameplayEventData Payload)
{
	if (Payload.TargetData.Num() == 0)
	{
		return;
	}

	if (!IsValid(AttackData.DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::OnHitResultEvent - DamageEffectClass is invalid"));
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::OnHitResultEvent - AbilitySystemComponent is invalid"));
		return;
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	if (!EffectContext.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::OnHitResultEvent - EffectContext is invalid"));
		return;
	}

	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		AttackData.DamageEffectClass,
		GetAbilityLevel(),
		EffectContext
	);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SkillE_Aurora::OnHitResultEvent - Failed to create GameplayEffectSpec"));
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Base, AttackData.BaseDamage);
	SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage_Multiplier, AttackData.DamageMultiplier);

	ApplyGameplayEffectSpecToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		SpecHandle,
		Payload.TargetData
	);
}

void UGA_SkillE_Aurora::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillE_Aurora::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SkillE_Aurora::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
