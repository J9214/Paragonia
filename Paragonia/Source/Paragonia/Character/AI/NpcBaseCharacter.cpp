// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/NpcBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PlayerState/PGPlayerState.h"
#include "AIController.h"

ANpcBaseCharacter::ANpcBaseCharacter()
	:TeamId(255),
	MaterialCounts(0)
{
	bReplicates = true;

	SetReplicateMovement(true);

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// Minimal : GameplayCue 와 Tag만 복제
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ANpcBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GrantStartupAbilities();
	}

	if (StateTreeComponent)
	{
		StateTreeComponent->StartLogic();
	}
}

void ANpcBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (AttributeSet)
		{
			AttributeSet->OnHealthChanged.AddDynamic(this, &ANpcBaseCharacter::OnHealthChanged);
		}
	}
}

UAbilitySystemComponent* ANpcBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANpcBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANpcBaseCharacter, TeamId);
}

void ANpcBaseCharacter::HandleDeath()
{
	if (AbilitySystemComponent && DeadTag.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StateTreeComponent->SendStateTreeEvent(DeadTag);
}

void ANpcBaseCharacter::SetTeamId(uint8 NewTeamId)
{
	if (HasAuthority())
	{
		TeamId = NewTeamId;

		OnRep_TeamId();
	}
}

void ANpcBaseCharacter::SetAttackTarget(AActor* NewTarget)
{
	if (HasAuthority())
	{
		CurrentAttackTarget = NewTarget;
	}
}

AActor* ANpcBaseCharacter::GetAttackTarget() const
{
	return CurrentAttackTarget.Get();
}

void ANpcBaseCharacter::GrantStartupAbilities()
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANpcBaseCharacter::GrantStartupAbilities - ASC is Not Valid!"));
		return;
	}

	if (StartupAbilities.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ANpcBaseCharacter::GrantStartupAbilities - StartupAbilities is Empty!"));
		return;
	}

	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(Spec);
	}
}

void ANpcBaseCharacter::OnHealthChanged(float OldValue, float NewValue)
{
	if (NewValue <= 0.0f &&
		OldValue > 0.0f)
	{
		HandleDeath();
	}
}

void ANpcBaseCharacter::OnRep_TeamId()
{
	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(this, 0);
	if (LocalPC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANpcBaseCharacter::OnRep_TeamId - LocalPC is Null?!"));
		return;
	}

	UMaterialInterface* MaterialToUse = nullptr;

	int32 MyTeamId = 255;
	if (APGPlayerState* PS = LocalPC->GetPlayerState<APGPlayerState>())
	{
		MyTeamId = PS->GetTeamID();
	}

	if (TeamId == MyTeamId)
	{
		MaterialToUse = AllyMaterial;
	}
	else
	{
		MaterialToUse = EnemyMaterial;
	}

	if (MaterialToUse && GetMesh())
	{
		GetMesh()->SetMaterial(MaterialCounts, MaterialToUse);
	}
}
