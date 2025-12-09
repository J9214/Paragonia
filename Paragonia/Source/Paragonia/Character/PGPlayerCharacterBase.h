#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Struct/FAttackData.h"
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
class PARAGONIA_API APGPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APGPlayerCharacterBase();

	UFUNCTION(Client, Reliable)
	void DrawDebugAttackCollision(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward, const FAttackData& AttackData);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void StartJump(const FInputActionValue& Value);

	void StopJump(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

private:
	void InitializeActorInfo();

	void InitializeAbilities();

	void InitializeAttributes();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

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

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;
};
