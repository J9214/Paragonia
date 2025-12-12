// PGGameMode.cpp

#include "PGGameModeBase.h"
#include "Character/PGPlayerCharacterBase.h"
#include "PlayerState/PGPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameState/PGGameStateBase.h"
#include "Controller/PGPlayerController.h"

APGGameModeBase::APGGameModeBase()
{
    // 기본 Pawn, PlayerController 클래스 설정은 여기서
}

// 로그인 시 플레이어 컨트롤러를 생존자 목록에 추가
void APGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    APGGameStateBase* PGGameState = GetGameState<APGGameStateBase>();
    if (IsValid(PGGameState) == false)
    {
        return;
    }

    APGPlayerController* NewPlayerController = Cast<APGPlayerController>(NewPlayer);
    if (IsValid(NewPlayerController) == true)
    {
        AlivePlayerControllers.Add(NewPlayerController);
    }
}

// 로그아웃 시 플레이어 컨트롤러를 생존자 목록에서 제거하고 사망자 목록에 추가
void APGGameModeBase::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    APGPlayerController* ExitingPlayerController = Cast<APGPlayerController>(Exiting);
    if (IsValid(ExitingPlayerController) == true && AlivePlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
    {
        AlivePlayerControllers.Remove(ExitingPlayerController);
        DeadPlayerControllers.Add(ExitingPlayerController);
    }
}

// 게임 시작 시 부활 체크 타이머 설정
void APGGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // 0.5초마다 부활 체크
    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &APGGameModeBase::TickRespawn,
        0.5f,
        true
    );
}

#pragma region DeathAndRespawn
// 캐릭터 사망 처리
void APGGameModeBase::HandleCharacterDeath(APGPlayerCharacterBase* DeadCharacter, AController* InstigatorController)
{
    if (!DeadCharacter)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("HandleCharacterDeath: %s died"),
        *GetNameSafe(DeadCharacter));


    AController* DeadController = DeadCharacter->GetController();
    APGPlayerState* VictimPS = DeadController ? DeadController->GetPlayerState<APGPlayerState>() : nullptr;
    APGPlayerState* KillerPS = InstigatorController ? InstigatorController->GetPlayerState<APGPlayerState>() : nullptr;

    float RespawnDelay = BaseRespawnTime;
    if (VictimPS)
    {
        RespawnDelay += 5;
    }

    if (DeadController)
    {
        // 🔹 여기: AController* → APGPlayerController* 로 캐스팅
        APGPlayerController* DeadPGController = Cast<APGPlayerController>(DeadController);
        if (DeadPGController)
        {
            // Alive 목록에서 제거
            AlivePlayerControllers.Remove(DeadPGController);

            // Dead 목록에 추가
            DeadPlayerControllers.AddUnique(DeadPGController);
        }

        // 부활 큐 등록
        FPGRespawnInfo Info;
        Info.Controller = DeadPGController;
        Info.RespawnTime = GetWorld()->GetTimeSeconds() + RespawnDelay;

        PendingRespawns.Add(Info);
        UE_LOG(LogTemp, Warning, TEXT("PendingRespawn Size %d"), PendingRespawns.Num());
    }
}

// 주기적으로 부활 대기열을 체크하고 부활 처리
void APGGameModeBase::TickRespawn()
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    UE_LOG(LogTemp, Warning, TEXT("CurrentTime %f"), CurrentTime);

    for (int32 i = PendingRespawns.Num() - 1; i >= 0; --i)
    {
        FPGRespawnInfo& Info = PendingRespawns[i];
        if (!Info.Controller.IsValid())
        {
            PendingRespawns.RemoveAt(i);
            continue;
        }

        if (CurrentTime >= Info.RespawnTime)
        {
            UE_LOG(LogTemp, Warning, TEXT("TickRespawn: Respawning %s"),
                *GetNameSafe(Info.Controller.Get()));

            RespawnPlayer(Info.Controller.Get());
            PendingRespawns.RemoveAt(i);
        }
    }
}

// 리스폰 캐릭터 상태 변경
void APGGameModeBase::RespawnPlayer(APGPlayerController* Controller)
{
    if (!Controller)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("RespawnPlayer: %s"), *GetNameSafe(Controller));

    if (APGPlayerCharacterBase* PGChar = Cast<APGPlayerCharacterBase>(Controller->GetPawn()))
    {
        PGChar->SetDeadState(0);
    }

    // 5) Alive / Dead 리스트 갱신
    DeadPlayerControllers.Remove(Controller);
    AlivePlayerControllers.AddUnique(Cast<APGPlayerController>(Controller));
}
#pragma endregion DeathAndRespawn