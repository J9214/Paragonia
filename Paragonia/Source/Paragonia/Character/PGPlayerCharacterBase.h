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
class UCharacterAttributeSet;
struct FInputActionValue;
struct FOnAttributeChangeData;

UCLASS()
class PARAGONIA_API APGPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APGPlayerCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Client, Reliable)
	void DrawDebugAttackCollision(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward, const FAttackData& AttackData);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetSpawnMoveLock(bool bLock);

	UFUNCTION(BlueprintCallable)
	UCharacterAttributeSet* GetCharacterAttributeSet() const { return CharacterAttributeSet; }

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void OnRep_Controller() override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void StartJump(const FInputActionValue& Value);

	void StopJump(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void SkillQ(const FInputActionValue& Value);

	void SkillE(const FInputActionValue& Value);

	void SkillR(const FInputActionValue& Value);

private:
	void InitializeActorInfo();

	void InitializeAbilities();

	void InitializeAttributes();

	void InitializeAttributesData();

	void BindAttributeChangeDelegates();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnDefenseChanged(const FOnAttributeChangeData& Data);

	void OnAttackPowerChanged(const FOnAttributeChangeData& Data);

	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UCharacterAttributeSet> CharacterAttributeSet;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SkillQAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SkillEAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SkillRAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	FName CharacterName;

private:
	bool bSpawnMoveLock;

#pragma region Respawn
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCSetDeadState(uint8 bDead);

	UFUNCTION()
	void SetDeadState(uint8 bDead); 
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Dead();

	void MovePlayerToRespawnPoint(FTransform SpawnTransform);

	FTransform GetRespawnLocationForController() const;

	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	uint8 bIsDead;
#pragma endregion

};
