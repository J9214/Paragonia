#include "GA/Sparrow/GA_SpawnBullet_Sparrow.h"

#include "Character/PGPlayerCharacterBase.h"
#include "TargetActor/PGTargetActor.h"
#include "Struct/BulletDataWrapper.h"
#include "Bullet/PGTaskRelatedBullet.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_SpawnBullet_Sparrow::UGA_SpawnBullet_Sparrow()
{
}

void UGA_SpawnBullet_Sparrow::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasAuthority(&ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SpawnBullet_Sparrow::ActivateAbility - No Authority"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SpawnBullet_Sparrow::ActivateAbility - CommitAbility Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SpawnBullet_Sparrow::ActivateAbility - No TriggerEventData"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const UBulletDataWrapper* Wrapper = Cast<UBulletDataWrapper>(TriggerEventData->OptionalObject);
	if (!Wrapper)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnBullet Ability: Wrapper is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentAttackData = Wrapper->Data;
	TSubclassOf<AActor> BulletClass = Wrapper->BulletClass;

	if (!IsValid(BulletClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("BulletClass: BulletClass is not valid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character: Character is not valid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UWorld* World = GetWorld();

	FTransform SpawnTransform = Wrapper->BulletSpawnTransform;
	SpawnTransform.SetRotation(FQuat(Character->FindComponentByClass<UCameraComponent>()->GetComponentRotation()));

	FActorSpawnParameters Param;
	Param.Owner = GetAvatarActorFromActorInfo();

	AActor* NewBullet = GetWorld()->SpawnActor(BulletClass, &SpawnTransform, Param);
	if (!IsValid(NewBullet))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_SpawnBullet_Sparrow: NewBullet is not valid"));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	APGTaskRelatedBullet* CreatingBullet = Cast<APGTaskRelatedBullet>(NewBullet);
	if (IsValid(CreatingBullet))
	{
		CreatingBullet->InitBullet(CurrentAttackData);
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_SpawnBullet_Sparrow::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
