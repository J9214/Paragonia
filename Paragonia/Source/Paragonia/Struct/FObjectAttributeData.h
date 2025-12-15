#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FObjectAttributeData.generated.h"

USTRUCT(BlueprintType)
struct FObjectAttributeData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 500.f;
};
