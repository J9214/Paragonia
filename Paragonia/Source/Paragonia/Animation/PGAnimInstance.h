#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Struct/FAttackData.h"
#include "PGAnimInstance.generated.h"

class UCharacterMovementComponent;

UCLASS()
class PARAGONIA_API UPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetCurrentAttackData(const FAttackData& NewAttackData);

protected:
	UFUNCTION()
	void AnimNotify_HitCheck();

	UFUNCTION()
	void AnimNotify_DashStart();

	UFUNCTION()
	void AnimNotify_DashStop();

	UFUNCTION()
	void AnimNotify_SpawnEnd();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDeath : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float YawDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFullBody : 1;

private:
	FRotator LastRotation;

	float LeanIntensity;

	FAttackData CurrentAttackData;
};
