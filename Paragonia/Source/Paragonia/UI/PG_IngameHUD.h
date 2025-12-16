// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_IngameHUD.generated.h"

class UCharacterAttributeSet;
class UPG_PlayerHPBar;
/**
 *
 */
UCLASS()
class PARAGONIA_API UPG_IngameHUD : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	bool CheckOwnerPlayerAttribute();

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandlePlayerHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandlePlayerMaxHealthChanged(float OldValue, float NewValue);


	UFUNCTION()
	void HandleTeam1HealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam1MaxHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam2HealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleTeam2MaxHealthChanged(float OldValue, float NewValue);

	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> OwnerPlayerAttributeSet;

	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> Team1AttributeSet;

	UPROPERTY()
	TObjectPtr<UCharacterAttributeSet> Team2AttributeSet;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_PlayerHPBar> PlayerHPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_PlayerHPBar> Team1HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPG_PlayerHPBar> Team2HPBar;
};
