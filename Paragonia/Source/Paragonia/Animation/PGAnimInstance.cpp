#include "Animation/PGAnimInstance.h"
#include "Character/PGPlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Struct/AttackDataWrapper.h"

void UPGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}

	LastRotation = OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
	LeanIntensity = 7.0f;
}

void UPGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacterMovementComponent))
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	const FRotator ActorRotation = OwnerCharacter->GetActorRotation();
	LocalVelocity = ActorRotation.UnrotateVector(Velocity);

	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();

	bShouldMove = (!OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) && (0.01f < GroundSpeed);

	FRotator AimRotation = (OwnerCharacter->GetBaseAimRotation() - OwnerCharacter->GetActorRotation()).GetNormalized();
	AimPitch = AimRotation.Pitch;
	AimYaw = AimRotation.Yaw;

	bIsFullBody = GetCurveValue(FName("FullBody"));

	FRotator DeltaRotation = (OwnerCharacter->GetActorRotation() - LastRotation).GetNormalized();
	YawDelta = FMath::FInterpTo(YawDelta, (DeltaRotation.Yaw / DeltaSeconds) / LeanIntensity, DeltaSeconds, 6.f);
	LastRotation = OwnerCharacter->GetActorRotation();
}

void UPGAnimInstance::SetCurrentAttackData(const FAttackData& NewAttackData)
{
	CurrentAttackData = NewAttackData;
}

void UPGAnimInstance::AnimNotify_HitCheck()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		UAttackDataWrapper* Wrapper = NewObject<UAttackDataWrapper>(this);
		Wrapper->Data = CurrentAttackData;

		FGameplayEventData EventData;
		EventData.Instigator = OwnerCharacter;
		EventData.OptionalObject = Wrapper;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OwnerCharacter,
			FGameplayTag::RequestGameplayTag(FName("Event.Character.HitCheck")),
			EventData
		);
	}
}

void UPGAnimInstance::AnimNotify_DashStart()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter,
		FGameplayTag::RequestGameplayTag("Event.Aurora.DashStart"),
		EventData
	);
}

void UPGAnimInstance::AnimNotify_SpawnEnd()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->SetSpawnMoveLock(false);
	}
}

void UPGAnimInstance::AnimNotify_ComboWindowOpened()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter,
		FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowOpened"),
		FGameplayEventData()
	);
}

void UPGAnimInstance::AnimNotify_ComboWindowClosed()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter,
		FGameplayTag::RequestGameplayTag("Event.Character.ComboWindowClosed"),
		FGameplayEventData()
	);
}

void UPGAnimInstance::AnimNotify_StartNextCombo()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter,
		FGameplayTag::RequestGameplayTag("Event.Character.StartNextCombo"),
		FGameplayEventData()
	);
}
