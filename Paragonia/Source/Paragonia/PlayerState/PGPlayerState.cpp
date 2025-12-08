#include "PlayerState/PGPlayerState.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Character/PGPlayerCharacterBase.h"

APGPlayerState::APGPlayerState()
{
	CharacterAttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

void APGPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AllAttributeSets.Add(CharacterAttributeSet);
}

UAbilitySystemComponent* APGPlayerState::GetAbilitySystemComponent() const
{
	APGPlayerCharacterBase* PlayerCharacter = Cast<APGPlayerCharacterBase>(GetPawn());
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetAbilitySystemComponent();
	}

	return nullptr;
}

TArray<UAttributeSet*> APGPlayerState::GetAllAttributeSets() const
{
	return AllAttributeSets;
}