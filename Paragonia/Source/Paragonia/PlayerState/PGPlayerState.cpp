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

	TeamType = ETeamType::Team1;
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


#pragma region Team

void APGPlayerState::OnRep_TeamType()
{
	// 팀 변경시 UI 갱신 등 처리
}

void APGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APGPlayerState, TeamType);
}

#pragma endregion Team