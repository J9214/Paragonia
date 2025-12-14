// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PGObject.h"
#include "PGNexus.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNexusDestroyed, AActor*, DestroyedActor);

/**
 * 
 */
UCLASS()
class PARAGONIA_API APGNexus : public APGObject
{
	GENERATED_BODY()

public:
	APGNexus();

protected:
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

public:
    UPROPERTY(BlueprintAssignable)
    FOnNexusDestroyed OnNexusDestroyed;



protected:
    // 오버랩 감지용 스피어 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nexus")
    USphereComponent* OverlapSphere;

    // 오버랩 시작 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

};
