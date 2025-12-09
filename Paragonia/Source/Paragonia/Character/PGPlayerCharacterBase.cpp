#include "Character/PGPlayerCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "PlayerState/PGPlayerState.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "Struct/FAttackData.h"

APGPlayerCharacterBase::APGPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void APGPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeActorInfo();

	if (HasAuthority())
	{
		InitializeAbilities();
		InitializeAttributes();
	}
}

void APGPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeActorInfo();

	APGPlayerState* PS = GetPlayerState<APGPlayerState>();
	if (!IsValid(PS))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributes - PlayerState is not valid"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void APGPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APGPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::StartJump);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::Attack);
	}
}

void APGPlayerCharacterBase::Move(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void APGPlayerCharacterBase::Look(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APGPlayerCharacterBase::StartJump(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FGameplayTag JumpTag = FGameplayTag::RequestGameplayTag(FName("Character.Ability.Jump"));
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(JumpTag));
}

void APGPlayerCharacterBase::StopJump(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	StopJumping();
}

void APGPlayerCharacterBase::Attack(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Ability.Attack"));
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
}

void APGPlayerCharacterBase::InitializeActorInfo()
{
	ASC->InitAbilityActorInfo(this, this);
}

void APGPlayerCharacterBase::InitializeAbilities()
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AllAbilities)
	{
		if (IsValid(AbilityClass))
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1, 0, this);
			ASC->GiveAbility(Spec);
		}
	}
}

void APGPlayerCharacterBase::InitializeAttributes()
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributes - ASC is not valid"));
		return;
	}

	APGPlayerState* PS = GetPlayerState<APGPlayerState>();
	if (!IsValid(PS))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributes - PlayerState is not valid"));
		return;
	}

	for (UAttributeSet* AttributeSet : PS->GetAllAttributeSets())
	{
		if (IsValid(AttributeSet))
		{
			ASC->AddAttributeSetSubobject(AttributeSet);
		}
	}
}

void APGPlayerCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		// Call Server RPC (Death Delegate broadcast on server)
	}

	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::OnHealthChanged - New Health: %f"), Data.NewValue);
}


UAbilitySystemComponent* APGPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void APGPlayerCharacterBase::DrawDebugAttackCollision_Implementation(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward, const FAttackData& AttackData)
{
	FVector Center = TraceStart + (TraceEnd - TraceStart) * 0.5f;

	switch (AttackData.SweepShape)
	{
		case EPGAttackShape::Sphere:
		{
			DrawDebugSphere(
				GetWorld(),
				Center,
				AttackData.Radius,
				16,
				DrawColor,
				false,
				5.0f
			);
		}
		break;

		case EPGAttackShape::Capsule:
		{
			float HalfHeight = AttackData.Range * 0.5f;

			DrawDebugCapsule(
				GetWorld(),
				Center,
				HalfHeight,
				AttackData.Radius,
				FRotationMatrix::MakeFromZ(Forward).ToQuat(),
				DrawColor,
				false,
				5.0f
			);
		}
		break;

		case EPGAttackShape::Box:
		{
			float HalfHeight = 88.f;

			if (ACharacter* Char = Cast<ACharacter>(this))
			{
				if (UCapsuleComponent* Capsule = Char->GetCapsuleComponent())
				{
					HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
				}
			}

			FVector Extent;
			Extent.X = AttackData.Range * 0.5f;
			Extent.Y = AttackData.Radius;
			Extent.Z = HalfHeight;

			DrawDebugBox(
				GetWorld(),
				Center,
				Extent,
				GetActorRotation().Quaternion(),
				DrawColor,
				false,
				5.0f
			);
		}
		break;
	}
}