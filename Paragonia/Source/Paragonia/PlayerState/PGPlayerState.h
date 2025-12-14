#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PGPlayerState.generated.h"

class UAttributeSet;
class UCharacterAttributeSet;

UENUM(BlueprintType)
enum class ETeamType : uint8 { None, Team1, Team2 };

UCLASS()
class PARAGONIA_API APGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APGPlayerState();

	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TArray<UAttributeSet*> GetAllAttributeSets() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<UCharacterAttributeSet> CharacterAttributeSet;

private:
	TArray<TObjectPtr<UAttributeSet>> AllAttributeSets;


#pragma region Team
public:
	UFUNCTION()
	void OnRep_TeamType();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TeamType)
	ETeamType TeamType = ETeamType::None;

#pragma endregion Team
};