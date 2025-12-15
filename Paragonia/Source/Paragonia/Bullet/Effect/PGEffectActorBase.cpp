#include "Bullet/Effect/PGEffectActorBase.h"

#include "Kismet/KismetMathLibrary.h"

APGEffectActorBase::APGEffectActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APGEffectActorBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentDuration = 0.0f;
}

void APGEffectActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform NewTransform = StartTransform;

	CurrentDuration += DeltaTime;
	float Ratio = CurrentDuration / Duration;

	FVector NewLocation = FMath::Lerp(StartTransform.GetLocation(), EndTransform.GetLocation(), Ratio);
	NewTransform.SetLocation(NewLocation);
	SetActorTransform(NewTransform);

	if (Ratio >= 1.0f)
	{
		Destroy();
	}
}

void APGEffectActorBase::InitEffect(const FTransform& InStart, const FTransform& InEnd, const float& InDuration)
{
	StartTransform = InStart;
	EndTransform = InEnd;
	Duration = InDuration;
}

