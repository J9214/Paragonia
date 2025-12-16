// Fill out your copyright notice in the Description page of Project Settings.


#include "PGObject.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/ObjectAttributeSet.h"
#include "Subsystem/PGObjectAttributeDataSubsystem.h"
#include "Struct/FObjectAttributeData.h"


// Sets default values
APGObject::APGObject()
{
	PrimaryActorTick.bCanEverTick = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ObjectAttributeSet = CreateDefaultSubobject<UObjectAttributeSet>(TEXT("ObjectAttributeSet"));

}

UAbilitySystemComponent* APGObject::GetAbilitySystemComponent() const
{
	return ASC;
}

// Called when the game starts or when spawned
void APGObject::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeActorInfo();

	if (HasAuthority())
	{
		InitializeAttributes();
		InitializeAttributesData();
	}
}


void APGObject::InitializeActorInfo()
{
	ASC->InitAbilityActorInfo(this, this);
}

void APGObject::InitializeAttributes()
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGObject::InitializeAttributes - ASC is not valid"));
		return;
	}

	ASC->AddAttributeSetSubobject<UObjectAttributeSet>(ObjectAttributeSet);

	BindAttributeChangeDelegates();
}

void APGObject::InitializeAttributesData()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(ObjectAttributeSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGObject::InitializeAttributesData - ObjectAttributeSet is not valid"));
		return;
	}

	UPGObjectAttributeDataSubsystem* AttributeSubsystem = GetGameInstance()->GetSubsystem<UPGObjectAttributeDataSubsystem>();
	if (!IsValid(AttributeSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("APGObject::InitializeAttributesData - AttributeSubsystem is not valid"));
		return;
	}

	const FObjectAttributeData* AttributeData = AttributeSubsystem->GetAttributeDataByName(ObjectName);
	if (AttributeData)
	{
		ObjectAttributeSet->InitMaxHealth(AttributeData->MaxHealth);
		ObjectAttributeSet->InitHealth(AttributeData->Health);
		ObjectAttributeSet->InitAttackPower(AttributeData->AttackPower);
		ObjectAttributeSet->InitMoveSpeed(AttributeData->MoveSpeed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APGObject::InitializeAttributesData - No AttributeData found for %s"), *ObjectName.ToString());
	}
}

void APGObject::BindAttributeChangeDelegates()
{
	if (!IsValid(ASC))
	{
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(UObjectAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void APGObject::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		HandleHealthDepleted();
	}
}

void APGObject::HandleHealthDepleted()
{
	if (HasAuthority())
	{
		Destroy();
	}
}
