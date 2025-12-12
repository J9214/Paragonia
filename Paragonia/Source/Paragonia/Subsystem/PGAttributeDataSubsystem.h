#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PGAttributeDataSubsystem.generated.h"

struct FCharacterAttributeData;

UCLASS()
class PARAGONIA_API UPGAttributeDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPGAttributeDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	const FCharacterAttributeData* GetAttributeDataByName(const FName& AttributeName) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> CharacterAttributeDataTable;
};
