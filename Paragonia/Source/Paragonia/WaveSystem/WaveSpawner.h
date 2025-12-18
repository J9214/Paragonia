// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Struct/NpcDataRow.h"
#include "WaveDefinition.h"
#include "WaveSpawner.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

UCLASS()
class PARAGONIA_API AWaveSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveSpawner();

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnWaveStart();

protected:
	void CacheDataTableTags();

	void HandleSpawnTick();

	void SpawnMinion(FName RowName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> PathSpline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Config")
	TObjectPtr<UWaveDefinition> CurrentWaveDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Config")
	TObjectPtr<UDataTable> WaveDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|GAS")
	TSubclassOf<UGameplayEffect> MinionInitEffectClass;

	// 이 Spawner에서 생성될 유닛의 TeamID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Config")
	uint8 TeamId;

private:
	TMap<FGameplayTag, FName> TagToRowMap;

	TArray<FName> SpawnNames;
	int32 CurrentSpawnIndex;

	FTimerHandle SpawnTimerHandle;
};
