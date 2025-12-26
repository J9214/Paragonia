#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Struct/FAttackData.h"
#include "GameplayTagContainer.h"
#include "Interface/PGTeamStatusInterface.h"
#include "PGPlayerCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCooldownTagChanged, FGameplayTag, CooldownTag, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCooldownTimeChanged, FGameplayTag, CooldownTag, float, TimeRemaining, float, Duration);

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputAction;
class UAbilitySystemComponent;
class UGameplayAbility;
class UWidgetComponent;
class UCharacterAttributeSet;
class UPG_IngameInfo;
class USceneCaptureComponent2D;
struct FInputActionValue;
struct FOnAttributeChangeData;

UCLASS()
class PARAGONIA_API APGPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface, public IPGTeamStatusInterface
{
	GENERATED_BODY()

public:
	APGPlayerCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Client, Reliable)
	void DrawDebugAttackCollision(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward, const FAttackData& AttackData);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetSpawningAbilityLock(bool bLock);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetInputLock(bool bLock);

	UFUNCTION(BlueprintCallable, Category="Cooldown")
    bool GetCooldownRemainingAndDurationByTag(FGameplayTag CooldownTag, float& OutRemaining, float& OutDuration) const;

	UFUNCTION(BlueprintCallable)
	UCharacterAttributeSet* GetCharacterAttributeSet() const { return CharacterAttributeSet; }

	UTextureRenderTarget2D* GetMinimapRenderTarget();

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

	void ToggleShopInput();

private:
	UFUNCTION()
	void OnCooldownTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void InitializeActorInfo();

	void InitializeAbilities();

	void InitializeAttributes();

	void InitializeAttributesData();

	void BindAttributeChangeDelegates();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnDefenseChanged(const FOnAttributeChangeData& Data);

	void OnAttackPowerChanged(const FOnAttributeChangeData& Data);

	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);

	void BindHeadHPDelegatesOnce();

	void SetupHeadHPWidget();

	void UpdateHeadHPVisibility();

	void OnAirborneTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void BindCooldownTagEvent();

	void StartCooldownTick(FGameplayTag CooldownTag);
	void StopCooldownTick(FGameplayTag CooldownTag);
	void TickCooldown(FGameplayTag CooldownTag);

public:
	UPROPERTY(BlueprintAssignable, Category = "Cooldown")
	FCooldownTagChanged OnCooldownTagChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Cooldown")
	FCooldownTimeChanged OnCooldownTimeChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> MiniMapSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> MinimapCaptureComponent;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleShop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	FName CharacterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HeadHPWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPG_IngameInfo> HeadHPWidgetClass;

	UPROPERTY()
	TObjectPtr<UPG_IngameInfo> HeadHPWidget;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> MinimapRT;

private:
	bool bInputLock;

	bool bHeadHPBound;

	TMap<FGameplayTag, FTimerHandle> CooldownTickTimerHandles;

#pragma region Respawn
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCSetDeadState(bool bDead);

	UFUNCTION()
	void SetDeadState(bool bDead); 

	virtual int32 GetTeamID_Implementation() const override; 
	virtual bool GetIsDead_Implementation() const { return bIsDead; }

	bool GetIsDead() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Dead();

	void MovePlayerToRespawnPoint(FTransform SpawnTransform);

	FTransform GetRespawnLocationForController() const;

	void ResetCharacterStateOnRespawn();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool bIsDead;
#pragma endregion

};
