// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PG_PlayerHPBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPG_PlayerHPBar : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Name;
};
