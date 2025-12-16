#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FCharacterAttributeData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAttributeData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 500.0f;
};
