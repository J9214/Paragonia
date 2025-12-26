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
#include <Kismet/KismetMathLibrary.h>

ANpcBaseCharacter::ANpcBaseCharacter()
	:TeamId(255),
	MaterialCounts(0),
	bIsDissolving(false),
	DeathAccumulatedTime(0.0f),
	DeathDuration(5.0f)
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
	StateTreeComponent->SetAutoActivate(true);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* ANpcBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANpcBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (StateTreeComponent)
	{
		StateTreeComponent->StartLogic();
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (AttributeSet)
		{
			AttributeSet->OnHealthChanged.AddDynamic(this, &ANpcBaseCharacter::OnHealthChanged);
		}

		if (HasAuthority())
		{
			GrantStartupAbilities();
		}
	}
}

void ANpcBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANpcBaseCharacter, TeamId);
}

void ANpcBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDissolving)
	{
		DeathAccumulatedTime += DeltaTime;

		float Alpha = FMath::Clamp(DeathAccumulatedTime / DeathDuration, 0.0f, 1.0f);

		for (UMaterialInstanceDynamic* MID : DissolveMaterials)
		{
			if (MID != nullptr)
			{
				MID->SetScalarParameterValue(FName("FadeOut"), Alpha);
			}
		}

		if (DeathAccumulatedTime >= DeathDuration)
		{
			bIsDissolving = false;
		}
	}
}

void ANpcBaseCharacter::HandleDeath()
{
	if (HasAuthority())
	{
		if (IsValid(AbilitySystemComponent) == true
			&& DeadTag.IsValid() == true)
		{
			AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		}

		if (StateTreeComponent)
		{
			StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(DeadTag));
		}
	}
}

void ANpcBaseCharacter::StartDeathEffect()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartDeathEffect - Called!"));
		Multicast_HandleDeath();
	}
}

void ANpcBaseCharacter::Multicast_HandleDeath_Implementation()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		const int32 MaterialCount = MeshComp->GetNumMaterials();
		for (int32 i = 0; i < MaterialCount; ++i)
		{
			UMaterialInstanceDynamic* MID = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
			if (MID != nullptr)
			{
				DissolveMaterials.Add(MID);
			}
		}
	}

	bIsDissolving = true;
	DeathAccumulatedTime = 0.0f;

	if (HasAuthority())
	{
		SetLifeSpan(DeathDuration + 0.1f);
	}
}

int32 ANpcBaseCharacter::GetTeamID_Implementation() const
{
	return TeamId;
}

bool ANpcBaseCharacter::GetIsDead_Implementation() const
{
	if (IsValid(AbilitySystemComponent) == false)
	{
		return false;
	}

	return AbilitySystemComponent->HasMatchingGameplayTag(DeadTag);
}

void ANpcBaseCharacter::SetTeamId(int32 NewTeamId)
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

bool ANpcBaseCharacter::CanAttack() const
{
	if (CurrentAttackTarget.IsValid() == false)
	{
		return false;
	}

	float DistSq = GetSquaredDistanceTo(CurrentAttackTarget.Get());
	float AttackRangeSq = AttackRange * AttackRange;

	return DistSq <= AttackRangeSq;
}

void ANpcBaseCharacter::SetAttackRange(float InRange)
{
	if (HasAuthority())
	{
		AttackRange = InRange;
	}
}

void ANpcBaseCharacter::SetSightRange(float InRange)
{
	if (HasAuthority())
	{
		SightRange = InRange;
	}
}

void ANpcBaseCharacter::SetRotationToTarget(AActor* TargetActor)
{
	if (IsValid(TargetActor) == false)
	{
		return;
	}

	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();

	MyLoc.Z = 0.0f;
	TargetLoc.Z = 0.0f;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);

	SetActorRotation(FRotator(0.0f, LookAtRot.Yaw, 0.0f));
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

	USkeletalMesh* MeshToUse = nullptr;

	int32 MyTeamId = 255;
	if (APGPlayerState* PS = LocalPC->GetPlayerState<APGPlayerState>())
	{
		if (IsValid(PS))
		{
			MyTeamId = PS->GetTeamID();
		}
	}

	if (TeamId == MyTeamId)
	{
		MeshToUse = AllyMesh;
	}
	else
	{
		MeshToUse = EnemyMesh;
	}

	if (MeshToUse && GetMesh())
	{
		// ABP 를 공유하기에 SkeltalMesh 에셋만 교체
		if (GetMesh()->GetSkeletalMeshAsset() != MeshToUse)
		{
			GetMesh()->SetSkeletalMeshAsset(MeshToUse);
		}
	}
}
