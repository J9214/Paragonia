#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct/FAttackData.h"
#include "AttackDataWrapper.generated.h"

UCLASS()
class PARAGONIA_API UAttackDataWrapper : public UObject
{
	GENERATED_BODY()
	
public:
	FAttackData Data;
};
