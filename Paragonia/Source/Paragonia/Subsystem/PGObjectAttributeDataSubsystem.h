// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PGObjectAttributeDataSubsystem.generated.h"

struct FObjectAttributeData;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPGObjectAttributeDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPGObjectAttributeDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	const FObjectAttributeData* GetAttributeDataByName(const FName& AttributeName) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> ObjectAttributeDataTable;
};
