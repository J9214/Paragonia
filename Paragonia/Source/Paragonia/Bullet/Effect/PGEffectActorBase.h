#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PGEffectActorBase.generated.h"

UCLASS()
class PARAGONIA_API APGEffectActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APGEffectActorBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void InitEffect(const FTransform& InStart, const FTransform& InEnd, const float& InDuration);

public:
	FTransform StartTransform;
	FTransform EndTransform;
	float Duration;

protected:
	float CurrentDuration;
};
