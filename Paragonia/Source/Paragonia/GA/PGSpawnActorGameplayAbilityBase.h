#pragma once

#include "CoreMinimal.h"
#include "GA/PGGameplayAbilityBase.h"
#include "PGSpawnActorGameplayAbilityBase.generated.h"

UCLASS()
class PARAGONIA_API UPGSpawnActorGameplayAbilityBase : public UPGGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Actor")
	TSubclassOf<AActor> SpawnActorClass;
};
