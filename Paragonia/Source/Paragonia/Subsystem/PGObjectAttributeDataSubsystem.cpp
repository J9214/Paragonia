#include "Subsystem/PGObjectAttributeDataSubsystem.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "Struct/FObjectAttributeData.h"

UPGObjectAttributeDataSubsystem::UPGObjectAttributeDataSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Attr(TEXT("/Game/Paragonia/Data/DT_ObjectAttribute.DT_ObjectAttribute"));
	if (DT_Attr.Succeeded())
	{
		ObjectAttributeDataTable = DT_Attr.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGObjectAttributeDataSubsystem::UPGObjectAttributeDataSubsystem - Failed to find ObjectAttributeDataTable at specified path."));
	}
}

void UPGObjectAttributeDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(ObjectAttributeDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPGObjectAttributeDataSubsystem::Initialize - ObjectAttributeDataTable is null. Check path."));
	}
}

void UPGObjectAttributeDataSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FObjectAttributeData* UPGObjectAttributeDataSubsystem::GetAttributeDataByName(const FName& AttributeName) const
{
	if (AttributeName.IsNone())
	{
		return nullptr;
	}

	if (!IsValid(ObjectAttributeDataTable))
	{
		return nullptr;
	}

	return ObjectAttributeDataTable->FindRow<FObjectAttributeData>(AttributeName, TEXT("UPGObjectAttributeDataSubsystem::GetAttributeData"));
}
