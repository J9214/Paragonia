// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Character/AI/NpcBaseCharacter.h"
#include "NpcDataRow.generated.h"

USTRUCT(BlueprintType)
struct FNpcDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ANpcBaseCharacter> SpawnNpcClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseAttack = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDefense = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag NPCTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NPCName;
};