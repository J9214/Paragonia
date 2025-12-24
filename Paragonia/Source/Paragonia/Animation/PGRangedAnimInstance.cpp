#include "Animation/PGRangedAnimInstance.h"

#include "Character/PGPlayerCharacterBase.h"
#include "Struct/BulletDataWrapper.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UPGRangedAnimInstance::AnimNotify_SpawnBullet()
{
	if (!OwnerCharacter->HasAuthority())
	{
		return;
	}

	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		UBulletDataWrapper* Wrapper = NewObject<UBulletDataWrapper>(this);
		Wrapper->Data = CurrentAttackData;
		Wrapper->BulletClass = BulletClass;
		Wrapper->TimeWaited = TimeWaited;
		Wrapper->BulletSpawnTransform = BulletSpawnTransform;

		FGameplayEventData EventData;
		EventData.Instigator = OwnerCharacter;
		EventData.OptionalObject = Wrapper;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			OwnerCharacter,
			FGameplayTag::RequestGameplayTag(FName("Event.Character.SpawnBullet")),
			EventData
		);
	}
}

void UPGRangedAnimInstance::AnimNotify_MoveLockOn()
{
	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
}

void UPGRangedAnimInstance::AnimNotify_MoveLockOff()
{
	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void UPGRangedAnimInstance::SetBulletClass(TSubclassOf<AActor> NewBulletClass)
{
	BulletClass = NewBulletClass;
}

void UPGRangedAnimInstance::SetTimeWaited(const float& InTimeWaited)
{
	TimeWaited = InTimeWaited;
}

void UPGRangedAnimInstance::SetBulletSpawnTransform(const FTransform& InTransform)
{
	BulletSpawnTransform = InTransform;
}
