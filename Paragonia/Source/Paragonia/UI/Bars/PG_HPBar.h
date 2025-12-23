// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_HPBar.generated.h"

class UProgressBar;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_HPBar : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void HandleMaxHealthChanged(float OldValue, float NewValue);

	UFUNCTION(BlueprintCallable)
	void AddBuff(int32 buffUID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BuffBox;

	UPROPERTY(EditDefaultsOnly, Category = "Buff")
	TSubclassOf<UUserWidget> BuffIconClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UUserWidget>> CreatedBuffs;

	UPROPERTY()
	float NowHPValue;
	UPROPERTY()
	float MaxHPValue;
};
