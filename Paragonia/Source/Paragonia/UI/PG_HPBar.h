// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_HPBar.generated.h"

class UProgressBar;
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

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY()
	float NowHPValue;
	UPROPERTY()
	float MaxHPValue;
};
