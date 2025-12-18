#include "Animation/Notify/PGLocalOnlySoundNottify.h"

#include "Character/PGPlayerCharacterBase.h"

void UPGLocalOnlySoundNottify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(MeshComp->GetOwner());
	if (!IsValid(PlayerCharacter)) return;

	if (PlayerCharacter->IsLocallyControlled())
	{
		Super::Notify(MeshComp, Animation, EventReference);
	}
}
