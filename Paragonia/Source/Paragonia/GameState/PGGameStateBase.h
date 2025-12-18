#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PGGameStateBase.generated.h"

UENUM(BlueprintType)
enum class ETeamResult : uint8 { None, Team1Win, Team2Win};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamResultChanged, ETeamResult, NewResult);

UCLASS()
class PARAGONIA_API APGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	APGGameStateBase();

#pragma region Team
public:
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TeamResult)
    ETeamResult TeamResult = ETeamResult::None;

    UPROPERTY(BlueprintAssignable)
    FOnTeamResultChanged OnTeamResultChanged;

    UFUNCTION()
    void OnRep_TeamResult();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma endregion Team
};
