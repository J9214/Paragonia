#include "Animation/PGRangedAnimInstance.h"

#include "Character/PGPlayerCharacterBase.h"
#include "Struct/BulletDataWrapper.h"

#include "AbilitySystemBlueprintLibrary.h"

void UPGRangedAnimInstance::AnimNotify_HitCheck()
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(OwnerCharacter);
	if (IsValid(PlayerCharacter))
	{
		UBulletDataWrapper* Wrapper = NewObject<UBulletDataWrapper>(this);
		Wrapper->Data = CurrentAttackData;
		Wrapper->BulletClass = BulletClass;

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

void UPGRangedAnimInstance::SetBulletClass(TSubclassOf<AActor> NewBulletClass)
{
	BulletClass = NewBulletClass;
}

void UPGRangedAnimInstance::SetTimeWaited(const float& InTimeWaited)
{
	TimeWaited = InTimeWaited;
}
