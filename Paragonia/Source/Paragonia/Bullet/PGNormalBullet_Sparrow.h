#pragma once

#include "CoreMinimal.h"
#include "Struct/FAttackData.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "PGNormalBullet_Sparrow.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UAbilityTask_WaitTargetData;

UCLASS()
class PARAGONIA_API APGNormalBullet_Sparrow : public AActor
{
	GENERATED_BODY()
	
public:	
	APGNormalBullet_Sparrow();

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void InitBullet(UGameplayAbility* InAbility, UAbilityTask_WaitTargetData* InTask, const FAttackData& InAttackData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> Projectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectClass;

	TObjectPtr<UGameplayAbility> Ability;
	TObjectPtr<UAbilityTask_WaitTargetData> Task;
	FAttackData AttackData;
};
