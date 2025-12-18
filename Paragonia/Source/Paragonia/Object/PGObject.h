// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "PGObject.generated.h"

class UAbilitySystemComponent;
class UCharacterAttributeSet;
struct FOnAttributeChangeData;

UCLASS()
class PARAGONIA_API APGObject : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APGObject();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	void InitializeActorInfo();

	void InitializeAttributes();

	void InitializeAttributesData();

	void BindAttributeChangeDelegates();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnDefenceChanged(const FOnAttributeChangeData& Data);

	void HandleHealthDepleted();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UCharacterAttributeSet> ObjectAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	FName ObjectName;
};
