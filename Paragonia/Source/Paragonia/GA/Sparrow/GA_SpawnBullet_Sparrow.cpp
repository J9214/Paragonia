#include "GA/Sparrow/GA_SpawnBullet_Sparrow.h"

#include "Character/PGPlayerCharacterBase.h"
#include "TargetActor/PGTargetActor.h"
#include "Struct/BulletDataWrapper.h"
#include "Bullet/PGNormalBullet_Sparrow.h"

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

	FAttackData AttackData = Wrapper->Data;
	TSubclassOf<AActor> BulletClass = Wrapper->BulletClass;

	if (!IsValid(BulletClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("BulletClass: BulletClass is not valid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FName SocketName = FName(TEXT("BowEmitterSocket"));
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character: Character is not valid"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UWorld* World = GetWorld();
	FTransform SpawnTransform = Character->GetMesh()->GetSocketTransform(SocketName);
	SpawnTransform.SetRotation(FQuat(Character->FindComponentByClass<UCameraComponent>()->GetComponentRotation()));
	FActorSpawnParameters Param;
	Param.Owner = GetAvatarActorFromActorInfo();
	AActor* NewBullet = GetWorld()->SpawnActor(BulletClass, &SpawnTransform, Param);
	if (!IsValid(NewBullet))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewBullet: NewBullet is not valid"));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	//EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);

	UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
			this,
			TEXT("SpawnBulletTask"),
			EGameplayTargetingConfirmation::Instant,
			APGTargetActor::StaticClass()
		);

	Task->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	Task->ReadyForActivation();

	APGNormalBullet_Sparrow* NormalBullet = Cast<APGNormalBullet_Sparrow>(NewBullet);
	if (IsValid(NormalBullet))
	{
		NormalBullet->InitBullet(this, Task, AttackData);
	}
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

void UGA_SpawnBullet_Sparrow::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	FGameplayEventData EventPayload;
	EventPayload.Instigator = GetAvatarActorFromActorInfo();
	EventPayload.TargetData = DataHandle;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetAvatarActorFromActorInfo(),
		FGameplayTag::RequestGameplayTag(FName("Event.Character.HitResult")),
		EventPayload
	);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SpawnBullet_Sparrow::OnTargetDataCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
