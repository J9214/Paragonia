#include "GA/Sparrow/GA_SkillQ_Sparrow.h"

#include "Bullet/Effect/PGEffectActorBase.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


UGA_SkillQ_Sparrow::UGA_SkillQ_Sparrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	TaskName = FName(TEXT("SkillQTask"));
	TargetingStartMontageSectionName = FName(TEXT("StartTargeting"));
	TargetingEndtMontageSectionName = FName(TEXT("EndTargeting"));
}

void UGA_SkillQ_Sparrow::CreateEffectActor()
{
	if (!IsValid(EffectActorClass)) return;

	FTransform StartTransform;
	FTransform EndTransform;

	UMeshComponent* Mesh = Cast<ACharacter>(GetAvatarActorFromActorInfo())->GetMesh();
	if (IsValid(Mesh))
	{
		StartTransform = Mesh->GetSocketTransform(FName(TEXT("BowEmitterSocket")));
	}

	EndTransform = SpawnTransform;
	EndTransform.SetLocation(EndTransform.GetLocation() + FVector(0.0f, 0.0f, 1000.0f));

	FRotator Rot = UKismetMathLibrary::FindLookAtRotation(StartTransform.GetLocation(), EndTransform.GetLocation());
	StartTransform.SetRotation(FQuat(Rot));

	AActor* NewEffect = GetWorld()->SpawnActor(EffectActorClass, &StartTransform);
	if (IsValid(NewEffect))
	{
		APGEffectActorBase* EffectActor = Cast<APGEffectActorBase>(NewEffect);
		if (IsValid(EffectActor))
		{
			EffectActor->InitEffect(StartTransform, EndTransform, 0.5f);
		}
	}
}
