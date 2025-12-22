// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponent.h"
#include "NpcBaseCharacter.generated.h"

class UAbilitySystemComponent;
class UCharacterAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class PARAGONIA_API ANpcBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ANpcBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTeamId(uint8 NewTeamId);

	UFUNCTION(BlueprintPure, Category = "AI")
	uint8 GetTeamId() const { return TeamId; }

	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	void SetAttackTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "AI|Target")
	AActor* GetAttackTarget() const;

	UFUNCTION(BlueprintPure, Category = "AI")
	float GetAttackRange() const { return AttackRange; };

	UFUNCTION(BlueprintPure, Category = "AI")
	float GetSightRange() const { return SightRange; };

	UFUNCTION(BlueprintPure, Category = "GAS|Tags")
	FGameplayTag GetDeadTag() const { return DeadTag; }

protected:
	void GrantStartupAbilities();

	UFUNCTION()
	void OnHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	virtual void OnRep_TeamId();

	virtual void HandleDeath();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCharacterAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|StateTree")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Tags")
	FGameplayTag DeadTag;

	UPROPERTY(ReplicatedUsing = OnRep_TeamId, EditAnywhere, BlueprintReadOnly, Category = "AI")
	uint8 TeamId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float SightRange;

	// 아군에게 보일 머테리얼
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Team")
	TObjectPtr<UMaterialInterface> AllyMaterial;

	// 적군에게 보일 머테리얼
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Team")
	TObjectPtr<UMaterialInterface> EnemyMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Visual|Team")
	int32 MaterialCounts;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Target")
	TWeakObjectPtr<AActor> CurrentAttackTarget;
};
