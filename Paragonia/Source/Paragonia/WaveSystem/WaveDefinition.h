// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveDefinition.generated.h"

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Wave|Minions")
	int32 WavePerMeleeCounts;

	UPROPERTY(EditDefaultsOnly, Category = "Wave|Minions")
	int32 WavePerRangeCounts;

	UPROPERTY(EditDefaultsOnly, Category = "Wave|Minions")
	int32 WavePerSeigeCounts;

	UPROPERTY(EditDefaultsOnly, Category = "Wave|Minions")
	int32 WavePerSuperCounts;

	UPROPERTY(EditDefaultsOnly, Category = "Wave|Minions")
	int32 SpawnRateSecond;
};

/**
 * 
 */
UCLASS()
class PARAGONIA_API UWaveDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FWaveData WaveData;
};
