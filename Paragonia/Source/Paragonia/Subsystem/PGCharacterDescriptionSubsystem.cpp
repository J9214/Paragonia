// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "PGCharacterDescriptionSubsystem.h"
#include "Struct/FCharacterDescription.h"
#include "Struct/FCharacterResourceInfo.h"
#include "Struct/FStatDescription.h"

UPGCharacterDescriptionSubsystem::UPGCharacterDescriptionSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Description(TEXT("/Game/Paragonia/Data/DT_CharacterDescriptiopn.DT_CharacterDescriptiopn"));
	if (DT_Description.Succeeded())
	{
		CharacterDescriptionDataTable = DT_Description.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGCharacterDescriptionSubsystem::UPGCharacterDescriptionSubsystem - Failed to find CharacterDescriptiopn at specified path."));
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ResourceInfo(TEXT("/Game/Paragonia/Data/DT_CharacterResourceInfo.DT_CharacterResourceInfo"));
	if (DT_ResourceInfo.Succeeded())
	{
		CharacterResourceInfoDataTable = DT_ResourceInfo.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGCharacterDescriptionSubsystem::UPGCharacterDescriptionSubsystem - Failed to find CharacterResourceInfo at specified path."));
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_StatDescription(TEXT("/Game/Paragonia/Data/DT_StatDescription.DT_StatDescription"));
	if (DT_StatDescription.Succeeded())
	{
		StatDescriptionDataTable = DT_StatDescription.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGCharacterDescriptionSubsystem::UPGCharacterDescriptionSubsystem - Failed to find StatDescription at specified path."));
	}
}

void UPGCharacterDescriptionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(CharacterDescriptionDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGCharacterDescriptionSubsystem::Initialize - CharacterDescriptionDataTable is null. Check path."));
		return;
	}

	if (!IsValid(CharacterResourceInfoDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGCharacterDescriptionSubsystem::Initialize - CharacterResourceInfoDataTable is null. Check path."));
		return;
	}

	const TMap<FName, uint8*>& RowMap = CharacterDescriptionDataTable->GetRowMap();
	for (const auto& Pair : RowMap)
	{
		const FName RowName = Pair.Key;
		const FCharacterResourceInfo* Row = reinterpret_cast<const FCharacterResourceInfo*>(Pair.Value);
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
	if (UID < 0)
		return nullptr;

	if (!IsValid(CharacterDescriptionDataTable))
		return nullptr;

	const FName* FoundRowName = UIDToRowName.Find(UID);
	if (!FoundRowName)
		return nullptr;

	return CharacterDescriptionDataTable->FindRow<FCharacterDescription>(*FoundRowName, TEXT("GetCharacterDescriptionByUID"));

}

const FCharacterResourceInfo* UPGCharacterDescriptionSubsystem::GetCharacterResource(const FName& CharacterName) const
{
	if (CharacterName.IsNone())
	{
		return nullptr;
	}

	if (!IsValid(CharacterResourceInfoDataTable))
	{
		return nullptr;
	}

	return CharacterResourceInfoDataTable->FindRow<FCharacterResourceInfo>(CharacterName, TEXT("GetCharacterResourceInfoByName"));

}

const FCharacterResourceInfo* UPGCharacterDescriptionSubsystem::GetCharacterResource(const int32 UID) const
{
	if (UID < 0) 
		return nullptr;

	if (!IsValid(CharacterResourceInfoDataTable))
		return nullptr;

	const FName* FoundRowName = UIDToRowName.Find(UID);
	if (!FoundRowName)
		return nullptr;

	return CharacterResourceInfoDataTable->FindRow<FCharacterResourceInfo>(*FoundRowName, TEXT("GetCharacterResourceInfoByUID"));
}

const FStatDescription* UPGCharacterDescriptionSubsystem::GetStatDescription(const FName& StatName) const
{
	if (StatName.IsNone())
	{
		return nullptr;
	}

	if (!IsValid(CharacterResourceInfoDataTable))
	{
		return nullptr;
	}

	return StatDescriptionDataTable->FindRow<FStatDescription>(StatName, TEXT("GetStatDescriptionByName"));

}

TArray<FName> UPGCharacterDescriptionSubsystem::GetAllRowNames() const
{
	if (!IsValid(CharacterDescriptionDataTable))
		return {};

	return CharacterDescriptionDataTable->GetRowNames();
}
