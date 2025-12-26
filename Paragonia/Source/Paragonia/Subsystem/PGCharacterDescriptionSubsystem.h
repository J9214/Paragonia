// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PGCharacterDescriptionSubsystem.generated.h"

struct FCharacterDescription;
struct FCharacterResourceInfo;
struct FStatDescription;
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPGCharacterDescriptionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPGCharacterDescriptionSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	const FCharacterDescription* GetCharacterDescription(const FName& CharacterName) const;
	const FCharacterDescription* GetCharacterDescription(const int32 UID) const;

	const FCharacterResourceInfo* GetCharacterResource(const FName& CharacterName) const;
	const FCharacterResourceInfo* GetCharacterResource(const int32 UID) const;

	const FStatDescription* GetStatDescription(const FName& StatName) const;

	TArray<FName> GetAllRowNames() const;
protected:

	UPROPERTY()
	TObjectPtr<UDataTable> CharacterDescriptionDataTable;

	UPROPERTY()
	TObjectPtr<UDataTable> CharacterResourceInfoDataTable;

	UPROPERTY()
	TObjectPtr<UDataTable> StatDescriptionDataTable;

	UPROPERTY(Transient)
	TMap<int32, FName> UIDToRowName;
};
