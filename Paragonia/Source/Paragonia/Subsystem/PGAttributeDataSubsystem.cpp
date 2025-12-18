#include "Subsystem/PGAttributeDataSubsystem.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "Struct/FCharacterAttributeData.h"

UPGAttributeDataSubsystem::UPGAttributeDataSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Attr(TEXT("/Game/Paragonia/Data/DT_CharacterAttribute.DT_CharacterAttribute"));
	if (DT_Attr.Succeeded())
	{
		CharacterAttributeDataTable = DT_Attr.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGAttributeDataSubsystem::UPGAttributeDataSubsystem - Failed to find CharacterAttributeDataTable at specified path."));
	}
}

void UPGAttributeDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(CharacterAttributeDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGAttributeDataSubsystem::Initialize - CharacterAttributeDataTable is null. Check path."));
	}
}

void UPGAttributeDataSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FCharacterAttributeData* UPGAttributeDataSubsystem::GetAttributeDataByName(const FName& AttributeName) const
{
	if (AttributeName.IsNone())
	{
		return nullptr;
	}

	if (!IsValid(CharacterAttributeDataTable))
	{
		return nullptr;
	}

	return CharacterAttributeDataTable->FindRow<FCharacterAttributeData>(AttributeName, TEXT("UPGAttributeDataSubsystem::GetAttributeData"));
}
