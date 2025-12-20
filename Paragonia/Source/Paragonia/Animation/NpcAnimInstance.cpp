// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NpcAnimInstance.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/PGGameplayTags.h"
#include "AbilitySystemComponent.h"

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

	UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		bIsDead = ASC->HasMatchingGameplayTag(OwnerCharacter->GetDeadTag());
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

	FGameplayEventData Payload;
	Payload.Instigator = OwnerCharacter;
	Payload.Target = CurrentTarget;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerCharacter, EventTag, Payload);
}