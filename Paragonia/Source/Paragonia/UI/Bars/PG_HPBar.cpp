// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bars/PG_HPBar.h"
#include "Components/Image.h"

void UPG_HPBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();


}


void UPG_HPBar::HandleHealthChanged(float OldValue, float NewValue)
{
	NowHPValue = NewValue;

	if (MaxHPValue == 0)
	{
		return;
	}

	float SetOldValue = OldValue / MaxHPValue;
	float SetNewValue = NewValue / MaxHPValue;

	UMaterialInstanceDynamic* BarFillMID = BarFill->GetDynamicMaterial();
	UMaterialInstanceDynamic* BarGlowMID = BarGlow->GetDynamicMaterial();
	
	if (!BarFillMID || !BarGlowMID)
	{
		return;
	}

	BarFillMID->SetScalarParameterValue(TEXT("HealthCurrent"), SetOldValue);
	BarFillMID->SetScalarParameterValue(TEXT("HealthUpdate"), SetNewValue);
	BarGlowMID->SetScalarParameterValue(TEXT("Health_Current"), SetOldValue);
	BarGlowMID->SetScalarParameterValue(TEXT("Health_Update"), SetNewValue);

	StopAnimation(Damaged);
	StopAnimation(Healed);

	if (OldValue > NewValue)
	{
		PlayAnimation(Damaged, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		PlayAnimation(Healed, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UPG_HPBar::HandleMaxHealthChanged(float OldValue, float NewValue)
{
	MaxHPValue = NewValue;

	if (MaxHPValue == 0)
	{
		return;
	}

	float SetOldValue = NowHPValue / OldValue;
	float SetNewValue = NowHPValue / NewValue;

	UMaterialInstanceDynamic* BarFillMID = BarFill->GetDynamicMaterial();
	UMaterialInstanceDynamic* BarGlowMID = BarGlow->GetDynamicMaterial();

	if (!BarFillMID || !BarGlowMID)
	{
		return;
	}

	BarFillMID->SetScalarParameterValue(TEXT("HealthCurrent"), SetOldValue);
	BarFillMID->SetScalarParameterValue(TEXT("HealthUpdate"), SetNewValue);
	BarGlowMID->SetScalarParameterValue(TEXT("Health_Current"), SetOldValue);
	BarGlowMID->SetScalarParameterValue(TEXT("Health_Update"), SetNewValue);

	StopAnimation(Damaged);
	StopAnimation(Healed);

	if (SetOldValue > SetNewValue)
	{
		PlayAnimation(Damaged, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		PlayAnimation(Healed, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}

	//HPBar->SetPercent(NewValue > 0.f ? NowHPValue / NewValue : 0.f);
}

void UPG_HPBar::AddBuff(int32 BuffUID)
{
}
