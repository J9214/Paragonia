#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "PGPlayerCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UAbilitySystemComponent;
class UGameplayAbility;
class UWidgetComponent;
struct FInputActionValue;
struct FOnAttributeChangeData;

UCLASS()
class PARAGONIA_API APGPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APGPlayerCharacterBase();


protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
};
