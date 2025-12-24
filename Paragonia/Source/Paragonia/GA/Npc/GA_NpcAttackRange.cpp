// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Npc/GA_NpcAttackRange.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "Bullet/Npc/NpcHomingProj.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UGA_NpcAttackRange::UGA_NpcAttackRange()
	:ProjectileSpeed(1500.0f),
	MuzzleSocketName(FName("Muzzle")),
	MaxWaitTime(10.0f)
{

}

void UGA_NpcAttackRange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (FireEventTag.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_NpcAttackRange::ActivateAbility - FireEventTag is UnValid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitFireTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FireEventTag,
		nullptr, false, false
	);

	if (IsValid(WaitFireTask) == true)
	{
		WaitFireTask->EventReceived.AddDynamic(this, &UGA_NpcAttackRange::OnFireEventReceived);
		WaitFireTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_NpcAttackRange::ActivateAbility - Failed to create HitResult Ability Task"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 시간이 과하게 지난 경우
	UAbilityTask_WaitDelay* TimeoutTask = UAbilityTask_WaitDelay::WaitDelay(this, MaxWaitTime);
	if (IsValid(TimeoutTask) == true)
	{
		TimeoutTask->OnFinish.AddDynamic(this, &UGA_NpcAttackRange::OnTimeOut);
		TimeoutTask->ReadyForActivation();
	}
}

void UGA_NpcAttackRange::OnMontageCompleted()
{
	// 몽타주 끝나도 GA 유지
}

void UGA_NpcAttackRange::OnAttackEventReceived(FGameplayEventData Payload)
{
	ApplyAttackDataEffects_OnHit(AttackData, Payload);

	// 부딪히면 그때 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_NpcAttackRange::OnFireEventReceived(FGameplayEventData Payload)
{
	if (GetOwningActorFromActorInfo()->HasAuthority() == false)
	{
		return;
	}

	if (IsValid(ProjectileClass) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_NpcAttackRange: ProjectileClass is missing!"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ANpcBaseCharacter* AvatarNPC = Cast<ANpcBaseCharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(AvatarNPC) == false)
	{
		return;
	}

	FTransform SpawnTransform;
	if (USkeletalMeshComponent* Mesh = AvatarNPC->GetMesh())
	{
		SpawnTransform = Mesh->GetSocketTransform(MuzzleSocketName);
	}
	else
	{
		SpawnTransform = AvatarNPC->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarNPC;
	SpawnParams.Instigator = AvatarNPC;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ANpcHomingProj* Projectile = GetWorld()->SpawnActor<ANpcHomingProj>(
		ProjectileClass,
		SpawnTransform,
		SpawnParams
	);

	if (IsValid(Projectile))
	{
		AActor* Target = AvatarNPC->GetAttackTarget();
		Projectile->InitializeProjectile(Target, ProjectileSpeed);
	}
}

void UGA_NpcAttackRange::OnTimeOut()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}