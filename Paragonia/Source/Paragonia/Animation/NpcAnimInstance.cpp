// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NpcAnimInstance.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/PGGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Interface/PGTeamStatusInterface.h"

UNpcAnimInstance::UNpcAnimInstance()
{
	GroundSpeed = 0.0f;
	bShouldMove = false;
	bIsDead = false;
}

void UNpcAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ANpcBaseCharacter>(TryGetPawnOwner());
	if (IsValid(OwnerCharacter))
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UNpcAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) == false
		|| IsValid(MovementComponent) == false)
	{
		return;
	}

	GroundSpeed = MovementComponent->Velocity.Size2D();
	bShouldMove = (GroundSpeed > 3.0f) && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);

	if (OwnerCharacter->GetClass()->ImplementsInterface(UPGTeamStatusInterface::StaticClass()))
	{
		bIsDead = IPGTeamStatusInterface::Execute_GetIsDead(OwnerCharacter);
	}
}

void UNpcAnimInstance::AnimNotify_AttackHit()
{
	SendGameplayEventToOwner(FGameplayTag::RequestGameplayTag(FName("Event.Montage.AttackHit")));
}

void UNpcAnimInstance::AnimNotify_ProjectileFire()
{
	SendGameplayEventToOwner(FGameplayTag::RequestGameplayTag(FName("Event.Montage.ProjectileFire")));
}

void UNpcAnimInstance::SendGameplayEventToOwner(FGameplayTag EventTag)
{
	if (IsValid(OwnerCharacter) == false)
	{
		return;
	}

	AActor* CurrentTarget = OwnerCharacter->GetAttackTarget();
	if (IsValid(CurrentTarget) == false)
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = OwnerCharacter;
	Payload.Target = CurrentTarget;

	FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit();
	NewData->HitResult = FHitResult(CurrentTarget, nullptr, FVector::ZeroVector, FVector::ZeroVector);
	NewData->HitResult.bBlockingHit = true;

	Payload.TargetData.Add(NewData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerCharacter, EventTag, Payload);
}