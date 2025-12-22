#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PGGreystoneSkillAura.generated.h"

UCLASS()
class PARAGONIA_API APGGreystoneSkillAura : public AActor
{
	GENERATED_BODY()
	
public:	
	APGGreystoneSkillAura();

protected:
	virtual void BeginPlay() override;

};
