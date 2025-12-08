#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PGPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class PARAGONIA_API APGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContexts;
};
