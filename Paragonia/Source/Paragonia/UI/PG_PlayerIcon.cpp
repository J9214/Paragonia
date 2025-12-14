// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_PlayerIcon.h"
#include "Struct/FCharacterDescription.h"
#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPG_PlayerIcon::NativeOnInitialized()
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

void UPG_PlayerIcon::ApplyIconTexture(UTexture2D* Tex)
{
	if (!IconImage || !Tex) return;

	IconImage->SetBrushFromTexture(Tex, true);
	IconImage->SetOpacity(0.5f);
}

void UPG_PlayerIcon::ApplyIcon()
{
	if (IconImage)
	{
		IconImage->SetOpacity(1.f);
	}
}

void UPG_PlayerIcon::SetPlayerIcon(const int32 CharacterUID)
{
	if (!CharacterDescSubsys)
	{
		if (UGameInstance* GI = GetGameInstance())
			CharacterDescSubsys = GI->GetSubsystem<UPGCharacterDescriptionSubsystem>();
	}

	if (!CharacterDescSubsys)
		return;

	const FCharacterDescription* Desc = CharacterDescSubsys->GetCharacterDescription(CharacterUID);
	if (!Desc)
		return;

	UTexture2D* Tex = nullptr;

	if (Desc->BoxIcon.IsValid())
		Tex = Desc->BoxIcon.Get();
	else
		Tex = Desc->BoxIcon.LoadSynchronous();

	ApplyIconTexture(Tex);
}

void UPG_PlayerIcon::SetPlayerIcon(const FName& CharacterName)
{
	if (!CharacterDescSubsys)
	{
		if (UGameInstance* GI = GetGameInstance())
			CharacterDescSubsys = GI->GetSubsystem<UPGCharacterDescriptionSubsystem>();
	}

	if (!CharacterDescSubsys)
		return;

	const FCharacterDescription* Desc = CharacterDescSubsys->GetCharacterDescription(CharacterName);
	if (!Desc)
		return;

	UTexture2D* Tex = nullptr;

	if (Desc->BoxIcon.IsValid())
		Tex = Desc->BoxIcon.Get();
	else
		Tex = Desc->BoxIcon.LoadSynchronous();

	ApplyIconTexture(Tex);
}

void UPG_PlayerIcon::SetPlayerNameText(const FText& InName)
{
	if (PlayerName)
	{
		PlayerName->SetText(InName);
	}
}

