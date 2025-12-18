#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "PGLocalOnlySoundNottify.generated.h"

UCLASS()
class PARAGONIA_API UPGLocalOnlySoundNottify : public UAnimNotify_PlaySound
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
