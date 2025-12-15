// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Struct/FStringTableData.h"
#include "PGStringTableSubsystem.generated.h"
/**
 * 
 */
UCLASS()
class PARAGONIA_API UPGStringTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPGStringTableSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "StringTable")
	FText GetText(ELanguageType Language, FName Key) const;
	

protected:

	UPROPERTY()
	TObjectPtr<UDataTable> StringTable;
};
