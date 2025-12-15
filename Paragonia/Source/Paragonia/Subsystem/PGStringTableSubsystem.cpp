// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/PGStringTableSubsystem.h"
#include "PGStringTableSubsystem.h"

UPGStringTableSubsystem::UPGStringTableSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_StringTable(TEXT("/Game/Paragonia/Data/DT_StringTable.DT_StringTable"));
	if (DT_StringTable.Succeeded())
	{
        StringTable = DT_StringTable.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGAttributeDataSubsystem::UPGAttributeDataSubsystem - Failed to find CharacterAttributeDataTable at specified path."));
	}
}

void UPGStringTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UPGStringTableSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

FText UPGStringTableSubsystem::GetText(ELanguageType Language, FName Key) const
{
    if (!StringTable)
    {
        return FText::GetEmpty();
    }

    const FStringTableData* Row = StringTable->FindRow<FStringTableData>(Key, TEXT("StringTableLookup"));

    if (!Row)
    {
        return FText::FromName(Key);
    }

    switch (Language)
    {
    case ELanguageType::English:
        return Row->English;
    case ELanguageType::Korean:
        return Row->Korean;
    default:
        return Row->English;
    }
}
