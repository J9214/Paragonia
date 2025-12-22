#include "Character/PGPlayerCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "PlayerState/PGPlayerState.h"
#include "AttributeSet/CharacterAttributeSet.h"
#include "GA/PGGameplayAbilityBase.h"
#include "Struct/FAttackData.h"
#include "GameMode/PGGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/PGAttributeDataSubsystem.h"
#include "Struct/FCharacterAttributeData.h"
#include "PlayerStart/PGPlayerStart.h"
#include "Components/WidgetComponent.h"
#include "UI/Panels/PG_IngameInfo.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

APGPlayerCharacterBase::APGPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false; 
	bHeadHPBound = false;

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
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;

	MiniMapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MiniMapSpringArm"));
	MiniMapSpringArm->SetupAttachment(RootComponent); 
	MiniMapSpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	MiniMapSpringArm->TargetArmLength = 1500.0f;
	MiniMapSpringArm->bUsePawnControlRotation = false;

	MinimapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCaptureComponent"));
	MinimapCaptureComponent->SetupAttachment(MiniMapSpringArm, USpringArmComponent::SocketName);
	MinimapCaptureComponent->bCaptureEveryFrame = false;
	MinimapCaptureComponent->bCaptureOnMovement = true;   
	MinimapCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CharacterAttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("CharacterAttributeSet"));

	HeadHPWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeadHPWidgetComp"));
	HeadHPWidgetComp->SetupAttachment(GetMesh());
	HeadHPWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	HeadHPWidgetComp->SetDrawAtDesiredSize(true);
	HeadHPWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 240.f));
	HeadHPWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APGPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeActorInfo();

	if (HasAuthority())
	{
		InitializeAbilities();
		InitializeAttributes();
		InitializeAttributesData();
	}

	UpdateHeadHPVisibility();
}

void APGPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void APGPlayerCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	InitializeActorInfo();
	BindAttributeChangeDelegates();

	FInputModeGameOnly InputMode;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetInputMode(InputMode);
	}

	SetupHeadHPWidget();
	UpdateHeadHPVisibility();
}

UTextureRenderTarget2D* APGPlayerCharacterBase::GetMinimapRenderTarget()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return nullptr;
	}

	MinimapRT = NewObject<UTextureRenderTarget2D>(this);
	MinimapRT->InitAutoFormat(512, 512);
	MinimapRT->ClearColor = FLinearColor::Black;
	MinimapRT->UpdateResourceImmediate(true);

	if (MinimapCaptureComponent)
	{
		MinimapCaptureComponent->TextureTarget = MinimapRT;
		MinimapCaptureComponent->CaptureScene();
	}
	return MinimapRT;
}

void APGPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	SetupHeadHPWidget();
	UpdateHeadHPVisibility();
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

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::Attack);
		EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Triggered, this, &ThisClass::SkillQ);
		EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Triggered, this, &ThisClass::SkillE);
		EnhancedInputComponent->BindAction(SkillRAction, ETriggerEvent::Triggered, this, &ThisClass::SkillR);
	}
}

void APGPlayerCharacterBase::Move(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || bInputLock)
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

	ASC->AbilityLocalInputPressed(static_cast<int32>(EPGAbilityInputID::Jump));
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

	ASC->AbilityLocalInputPressed(static_cast<int32>(EPGAbilityInputID::Attack));
}

void APGPlayerCharacterBase::SkillQ(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ASC->AbilityLocalInputPressed(static_cast<int32>(EPGAbilityInputID::SkillQ));
}

void APGPlayerCharacterBase::SkillE(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ASC->AbilityLocalInputPressed(static_cast<int32>(EPGAbilityInputID::SkillE));
}

void APGPlayerCharacterBase::SkillR(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ASC->AbilityLocalInputPressed(static_cast<int32>(EPGAbilityInputID::SkillR));
}

void APGPlayerCharacterBase::InitializeActorInfo()
{
	ASC->InitAbilityActorInfo(this, this);
}

void APGPlayerCharacterBase::InitializeAbilities()
{
	if (!IsValid(ASC))
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : AllAbilities)
	{
		if (!IsValid(AbilityClass))
		{
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<UGameplayAbility>();
		int32 InputID = 0;

		const UPGGameplayAbilityBase* PGAbility = Cast<UPGGameplayAbilityBase>(AbilityCDO);
		if (IsValid(PGAbility))
		{
			InputID = static_cast<int32>(PGAbility->InputID);
		}

		FGameplayAbilitySpec Spec(AbilityClass, 1, InputID, this);
		if (!ASC->FindAbilitySpecFromClass(AbilityClass))
		{
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

	ASC->AddAttributeSetSubobject<UCharacterAttributeSet>(CharacterAttributeSet);

	BindAttributeChangeDelegates();
}

void APGPlayerCharacterBase::InitializeAttributesData()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(CharacterAttributeSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributesData - CharacterAttributeSet is not valid"));
		return;
	}
	
	UPGAttributeDataSubsystem* AttributeSubsystem = GetGameInstance()->GetSubsystem<UPGAttributeDataSubsystem>();
	if (!IsValid(AttributeSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributesData - AttributeSubsystem is not valid"));
		return;
	}

	const FCharacterAttributeData* AttributeData = AttributeSubsystem->GetAttributeDataByName(CharacterName);
	if (AttributeData)
	{
		CharacterAttributeSet->InitMaxHealth(AttributeData->MaxHealth);
		CharacterAttributeSet->InitHealth(AttributeData->Health);
		CharacterAttributeSet->InitDefense(AttributeData->Defense);
		CharacterAttributeSet->InitAttackPower(AttributeData->AttackPower);
		CharacterAttributeSet->InitMoveSpeed(AttributeData->MoveSpeed);
		GetCharacterMovement()->MaxWalkSpeed = AttributeData->MoveSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::InitializeAttributesData - No AttributeData found for %s"), *CharacterName.ToString());
	}
}

void APGPlayerCharacterBase::BindAttributeChangeDelegates()
{
	if (!IsValid(ASC))
	{
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetDefenseAttribute()).AddUObject(this, &ThisClass::OnDefenseChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetAttackPowerAttribute()).AddUObject(this, &ThisClass::OnAttackPowerChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::OnMoveSpeedChanged);

}

void APGPlayerCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		if (bIsDead == 0.0f) {
			ServerRPCSetDeadState(true);
		}
	}
}

void APGPlayerCharacterBase::OnDefenseChanged(const FOnAttributeChangeData& Data)
{
}

void APGPlayerCharacterBase::OnAttackPowerChanged(const FOnAttributeChangeData& Data)
{
}

void APGPlayerCharacterBase::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void APGPlayerCharacterBase::BindHeadHPDelegatesOnce()
{
	if (bHeadHPBound)
	{
		return;
	}

	if (!IsValid(CharacterAttributeSet) || !IsValid(HeadHPWidget))
	{
		return;
	}

	CharacterAttributeSet->OnHealthChanged_UI.RemoveDynamic(HeadHPWidget, &UPG_IngameInfo::HandleHealthChanged);
	CharacterAttributeSet->OnMaxHealthChanged_UI.RemoveDynamic(HeadHPWidget, &UPG_IngameInfo::HandleMaxHealthChanged);

	CharacterAttributeSet->OnHealthChanged_UI.AddDynamic(HeadHPWidget, &UPG_IngameInfo::HandleHealthChanged);
	CharacterAttributeSet->OnMaxHealthChanged_UI.AddDynamic(HeadHPWidget, &UPG_IngameInfo::HandleMaxHealthChanged);

	HeadHPWidget->HandleMaxHealthChanged(CharacterAttributeSet->GetMaxHealth(), CharacterAttributeSet->GetMaxHealth());
	HeadHPWidget->HandleHealthChanged(CharacterAttributeSet->GetHealth(), CharacterAttributeSet->GetHealth());

	bHeadHPBound = true;
}

void APGPlayerCharacterBase::SetupHeadHPWidget()
{
	if (!IsValid(HeadHPWidgetComp))
	{
		return;
	}
	if (!HeadHPWidgetClass)
	{
		return;
	}

	if (HeadHPWidgetComp->GetWidgetClass() != HeadHPWidgetClass)
	{
		HeadHPWidgetComp->SetWidgetClass(HeadHPWidgetClass);
	}

	HeadHPWidgetComp->InitWidget();

	HeadHPWidget = Cast<UPG_IngameInfo>(HeadHPWidgetComp->GetUserWidgetObject());

	if (!IsValid(HeadHPWidget))
	{
		return;
	}

	BindHeadHPDelegatesOnce();
}

void APGPlayerCharacterBase::UpdateHeadHPVisibility()
{
	if (!IsValid(HeadHPWidgetComp))
	{
		return;
	}

	const bool bHide = IsLocallyControlled();
	HeadHPWidgetComp->SetHiddenInGame(bHide);
	HeadHPWidgetComp->SetVisibility(!bHide);
}

UAbilitySystemComponent* APGPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void APGPlayerCharacterBase::SetSpawningAbilityLock(bool bLock)
{
	if (bLock)
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Spawning")));
		SetInputLock(true);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Spawning")));
		SetInputLock(false);
	}
}

void APGPlayerCharacterBase::SetInputLock(bool bLock)
{
	bInputLock = bLock;
	if (bInputLock)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
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
				3.0f
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
				3.0f
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
				3.0f
			);
		}
		break;
	}
}

#pragma region Respawn
void APGPlayerCharacterBase::ServerRPCSetDeadState_Implementation(uint8 bDead)
{
	SetDeadState(bDead);
}

void APGPlayerCharacterBase::OnRep_Dead()
{
	if (bIsDead == 1) 
	{ // 사망

		if (APGGameModeBase* GM = GetWorld()->GetAuthGameMode<APGGameModeBase>())
		{
			GM->HandleCharacterDeath(this, GetController());
		}

		// 1) 입력 막기 
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);
		}

		// 2) 이동 막기 
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			// 완전 정지
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
			MoveComp->GravityScale = 0.0f;
		}

		// 3) Capsule, Disable Collision
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		if (IsValid(CapsuleComp))
		{
			CapsuleComp->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}
	else { // 리스폰

		UE_LOG(LogTemp, Warning, TEXT("OnRespawn: %s"), *GetNameSafe(this));

		FTransform SpawnTransform = GetRespawnLocationForController();
		MovePlayerToRespawnPoint(SpawnTransform);

		// 2) 메쉬 다시 보여주기 + 자식까지 전부
		if (USkeletalMeshComponent* SkelMesh = GetMesh())
		{
			SkelMesh->SetVisibility(true, true);
			SkelMesh->SetSimulatePhysics(false);

			GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			GetMesh()->SetRelativeLocation({ 0, 0, -90 });
			GetMesh()->SetRelativeRotation({ 0, -90, 0 });

		}

		// 3) 캡슐, 충돌 다시 활성화
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("Pawn"));
		}

		// 4) 이동 다시 켜기	
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
			MoveComp->GravityScale = 1.0f;
		}

		// 5) 입력 활성화
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			EnableInput(PC);
		}

		// 6) Reset Character State
		ResetCharacterStateOnRespawn();
	}
}

void APGPlayerCharacterBase::SetDeadState(uint8 bDead)
{
	if (bIsDead == bDead)
	{
		return;
	}

	bIsDead = bDead;

	OnRep_Dead();
}

uint8 APGPlayerCharacterBase::GetIsDead() const
{
	return bIsDead;
}

void APGPlayerCharacterBase::MovePlayerToRespawnPoint(FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector))
{
	FVector SpawnLocation = SpawnTransform.GetLocation();
	FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();

	this->TeleportTo(SpawnLocation, SpawnRotation, false, true);
}

FTransform APGPlayerCharacterBase::GetRespawnLocationForController() const
{
	AController* PlayerController = GetController();
	if (!PlayerController) return FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

	// PlayerState에서 팀ID 받기
	APGPlayerState* PS = PlayerController->GetPlayerState<APGPlayerState>();
	if (!PS) return FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

	int32 TeamID = PS->GetTeamID();

	// GameMode나 다른 매니저에서 팀별 스폰 위치 쿼리
	APGGameModeBase* GM = GetWorld()->GetAuthGameMode<APGGameModeBase>();
	if (GM)
	{
		FTransform TeamSpawnTransform = GM->GetTeamSpawnTransform(TeamID);
		return TeamSpawnTransform;
	}

	return FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
}

void APGPlayerCharacterBase::ResetCharacterStateOnRespawn()
{
	if (IsValid(CharacterAttributeSet))
	{
		CharacterAttributeSet->SetHealth(CharacterAttributeSet->GetMaxHealth());
	}
}

void APGPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APGPlayerCharacterBase, bIsDead);
}

#pragma endregion