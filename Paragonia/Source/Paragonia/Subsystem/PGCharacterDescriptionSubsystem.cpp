// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "PGCharacterDescriptionSubsystem.h"
#include "Struct/FCharacterDescription.h"

UPGCharacterDescriptionSubsystem::UPGCharacterDescriptionSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Description(TEXT("/Game/Paragonia/Data/DT_CharacterDescriptiopn.DT_CharacterDescriptiopn"));
	if (DT_Description.Succeeded())
	{
		CharacterDescriptionDataTable = DT_Description.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGAttributeDataSubsystem::UPGAttributeDataSubsystem - Failed to find CharacterAttributeDataTable at specified path."));
	}
}

void UPGCharacterDescriptionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(CharacterDescriptionDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGAttributeDataSubsystem::Initialize - CharacterAttributeDataTable is null. Check path."));
		return;
	}

	const TMap<FName, uint8*>& RowMap = CharacterDescriptionDataTable->GetRowMap();
	for (const auto& Pair : RowMap)
	{
		const FName RowName = Pair.Key;
		const FCharacterDescription* Row = reinterpret_cast<const FCharacterDescription*>(Pair.Value);
		if (!Row) continue;

		UIDToRowName.Add(Row->UID, RowName); // 중복 체크는 필요하면 추가
	}
}

void UPGCharacterDescriptionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FCharacterDescription* UPGCharacterDescriptionSubsystem::GetCharacterDescription(const FName& CharacterName) const
{
	if (CharacterName.IsNone())
	{
		return nullptr;
	}

	if (!IsValid(CharacterDescriptionDataTable))
	{
		return nullptr;
	}

	return CharacterDescriptionDataTable->FindRow<FCharacterDescription>(CharacterName, TEXT("GetCharacterDescriptionByName"));

}

const FCharacterDescription* UPGCharacterDescriptionSubsystem::GetCharacterDescription(const int32 UID) const
{
	if (UID <= 0) // UID 규칙 없으면 이 체크는 빼도 됨
		return nullptr;

	if (!IsValid(CharacterDescriptionDataTable))
		return nullptr;

	const FName* FoundRowName = UIDToRowName.Find(UID);
	if (!FoundRowName)
		return nullptr;

	return CharacterDescriptionDataTable->FindRow<FCharacterDescription>(*FoundRowName, TEXT("GetCharacterDescriptionByUID"));

}

TArray<FName> UPGCharacterDescriptionSubsystem::GetAllRowNames() const
{
	if (!IsValid(CharacterDescriptionDataTable))
		return {};

	return CharacterDescriptionDataTable->GetRowNames();
}
