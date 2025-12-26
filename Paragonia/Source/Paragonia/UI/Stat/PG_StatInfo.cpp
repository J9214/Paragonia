// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Stat/PG_StatInfo.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "Struct/FStatDescription.h"

void UPG_StatInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UGameInstance* GI = GetGameInstance())
	{
		CharacterDescSubsys = GI->GetSubsystem<UPGCharacterDescriptionSubsystem>();
	}

	if (!CharacterDescSubsys)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPG_PlayerIcon: CharacterDescSubsys is null."));
	}
}

bool UPG_StatInfo::SetStat(const FName& StatName, float Value)
{
	if (!CharacterDescSubsys)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			CharacterDescSubsys = GI->GetSubsystem<UPGCharacterDescriptionSubsystem>();
		}
	}

	if (!CharacterDescSubsys)
	{
		return false;
	}

	const FStatDescription* StatInfo = CharacterDescSubsys->GetStatDescription(StatName);
	if (!StatInfo)
	{
		return false;
	}

	UTexture2D* Tex = nullptr;

	if (StatInfo->StatIcon.IsValid())
	{
		Tex = StatInfo->StatIcon.Get();
	}
	else
	{
		Tex = StatInfo->StatIcon.LoadSynchronous();
	}

	if (!IconImage || !StatValue || !Tex)
	{
		return false;
	}

	FNumberFormattingOptions FormatOptions;
	FormatOptions.MinimumFractionalDigits = 0;
	FormatOptions.MaximumFractionalDigits = 0;

	StatValue->SetText(FText::AsNumber(Value, &FormatOptions));
	IconImage->SetBrushFromTexture(Tex, true);

	return true;
}

void UPG_StatInfo::ChangeValue(float Value)
{
	if (StatValue)
	{
		FNumberFormattingOptions FormatOptions;
		FormatOptions.MinimumFractionalDigits = 0;
		FormatOptions.MaximumFractionalDigits = 0;

		StatValue->SetText(FText::AsNumber(Value, &FormatOptions));
	}
}
