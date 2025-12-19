// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterSelectWidget.h"
#include "CommonTileView.h"
#include "UI/PG_CharacterDescription.h"
#include "Components/CanvasPanelSlot.h"
#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "Engine/GameInstance.h"
#include "Struct/CharacterInfoWrapper.h"
#include "PG_PlayerIcon.h"
#include "Components/Button.h"
#include "PG_CharacterSelectButton.h"
#include "PlayerState/LobbyPlayerState.h"
#include "Character/PG_LobbyPreviewCharacter.h"
#include "Pawn/LobbyPawn.h"

void UPG_CharacterSelectWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (UGameInstance* GI = GetGameInstance())
    {
        CharacterDescSubsys = GI->GetSubsystem<UPGCharacterDescriptionSubsystem>();
    }

    if (!CharacterDescSubsys)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterDescSubsys is null (PGCharacterDescriptionSubsystem)."));
        return;
    }

    if (CharacterTileView)
    {
        CharacterTileView->OnItemClicked().AddUObject(this, &ThisClass::HandleCharacterItemClicked);
    }

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &ThisClass::HandlePlayerReadyClicked);
        ReadyButton->SetIsEnabled(false);
    }

    InitCharacterList();
}

void UPG_CharacterSelectWidget::SetInit()
{
    CheckPlayerState();
    SpawnPreviewActorIfNeeded();

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            if (ALobbyPawn* LobbyChar = Cast<ALobbyPawn>(Pawn))
            {
                LobbyChar->StartMoveToTarget();
            }
        }
    }
}


void UPG_CharacterSelectWidget::SetPlayerNumberID(int32 index, int32 PlayerNumberID)
{
    if (Player0Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return;
    }
    if (Player1Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return;
    }
    if (Player2Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return;
    }

    switch (index)
    {
    case 0:
        Player0Icon->SetPlayerNumberId(PlayerNumberID);
        return;
    case 1:
        Player1Icon->SetPlayerNumberId(PlayerNumberID);
        return;
    case 2:
        Player2Icon->SetPlayerNumberId(PlayerNumberID);
        return;
    default:
        break;
    }
}

void UPG_CharacterSelectWidget::SetPlayerCharacterIcon(int32 PlayerNumberID, int32 CharacterUID)
{
    if (Player0Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        Player0Icon->SetPlayerIcon(CharacterUID);
        return;
    }
    if (Player1Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        Player1Icon->SetPlayerIcon(CharacterUID);
        return;
    }
    if (Player2Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        Player2Icon->SetPlayerIcon(CharacterUID);
        return;
    }

}

int32 UPG_CharacterSelectWidget::SetPlayerReady(int32 PlayerNumberID)
{
    if (Player0Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return Player0Icon->ApplyIcon();
    }
    if (Player1Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return Player1Icon->ApplyIcon();
    }
    if (Player2Icon->GetPlayerNumberId() == PlayerNumberID)
    {
        return Player2Icon->ApplyIcon();
    }

    return -1;
}

bool UPG_CharacterSelectWidget::SpawnPreviewActorIfNeeded()
{
    if (!PreviewActorClass)
        return false;

    if (PreviewActorInstance)
        return true;

    if (UWorld* World = GetWorld())
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PreviewActorInstance = World->SpawnActor<APG_LobbyPreviewCharacter>(
            PreviewActorClass,
            SpawnLocation,
            SpawnRotation,
            Params
        );

        PreviewActorInstance->SetActorHiddenInGame(true);
        return true;
    }

    return false;
}

void UPG_CharacterSelectWidget::HandleCharacterItemClicked(UObject* Item)
{
    if (!Item || !CharacterDescSubsys || !CharacterDescription)
        return;

    UCharacterInfoWrapper* Entry = Cast<UCharacterInfoWrapper>(Item);
    if (!Entry)
        return;

    ReadyButton->SetIsEnabled(true);

    const FCharacterDescription* DetailRow = CharacterDescSubsys->GetCharacterDescription(Entry->RowName);

    if (!DetailRow)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character row '%s' not found in CharacterDataTable"), *(Entry->RowName).ToString());
        return;
    }

    TArray<UObject*> Items = CharacterTileView->GetListItems();
    for (UObject* ObjectItem : Items)
    {
        if (UCharacterInfoWrapper* RowItem = Cast<UCharacterInfoWrapper>(ObjectItem))
        {
            int RowUID = RowItem->DescriptionData.UID;
            RowItem->bSelected = (RowUID == Entry->DescriptionData.UID);
        }
    }

    RefreshCharacterTileView();

    if (CheckPlayerState())
    {
        LobbyPlayerState->ServerSetCharacterID(Entry->DescriptionData.UID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerState is not ready yet!"));
    }

    CharacterDescription->InitDescription(*DetailRow);
    SelectedCharacterUID = Entry->DescriptionData.UID;

    if (SpawnPreviewActorIfNeeded())
    {
        USkeletalMesh* Mesh = Entry->ResourceData.LobbyMesh.LoadSynchronous();
        UAnimMontage* IntroMontage = Entry->ResourceData.IntroMontage.LoadSynchronous();
        UClass* ABP = Entry->ResourceData.LobbyAnimBP.LoadSynchronous();

        if (Mesh && IntroMontage && ABP)
        {
            PreviewActorInstance->SetActorHiddenInGame(false);
            PreviewActorInstance->SetCharacterMesh(Mesh, ABP);
            PreviewActorInstance->PlayMontage(IntroMontage);
        }
    }

    if (Player0Icon)
    {
        Player0Icon->SetPlayerIcon(Entry->RowName);
    }

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CharacterDescription->Slot))
    {
        const FVector2D CurrentPos = CanvasSlot->GetPosition();

        if (!CurrentPos.Equals(DescriptionPos, 0.1f))
        {
            CanvasSlot->SetPosition(DescriptionPos);
        }
    }
}
    

void UPG_CharacterSelectWidget::HandlePlayerReadyClicked()
{
    if (CheckPlayerState())
    {
        LobbyPlayerState->ServerSetLobbyState(EPlayerLobbyState::PLS_SelectedAndReady);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("PlayerState is not ready yet!"));

    TArray<UObject*> Items = CharacterTileView->GetListItems();

    for (UObject* Item : Items)
    {
        if (auto* RowItem = Cast<UCharacterInfoWrapper>(Item))
        {
            RowItem->bPlayerSelected = (RowItem->DescriptionData.UID == SelectedCharacterUID);
            RowItem->bCheckCanSelected = false;
        }
    }

    if (Player0Icon)
    {
        Player0Icon->ApplyIcon();
    }

    RefreshCharacterTileView();
    ReadyButton->SetIsEnabled(false);
}

void UPG_CharacterSelectWidget::HandleAnyCharacterIDChanged(int32 NewCharacterID)
{
    ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
    for (APlayerState* PS : GS->PlayerArray)
    {
        ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);

        if (!LPS)
        {
            continue;
        }
        if (LPS == LobbyPlayerState)
        {
            continue;
        }
        if (LPS->GetTeamID() != LobbyPlayerState->GetTeamID())
        {
            continue;
        }
        if (LPS->GetPlayerLobbyState() == EPlayerLobbyState::PLS_SelectedAndReady)
        {
            continue;
        }

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                FString::Printf(TEXT("[CLIENT] %d"), LobbyPlayerState->GetTeamID()));
        }
        SetPlayerCharacterIcon(LPS->GetPlayerNumberId(), LPS->GetCharacterID());
    }
}

void UPG_CharacterSelectWidget::HandlePlayerSelected(EPlayerLobbyState LobbyState, int32 PlayerUID)
{
    if (LobbyState != EPlayerLobbyState::PLS_SelectedAndReady)
        return;
    int32 SelectedCharacterID = SetPlayerReady(PlayerUID);

    TArray<UObject*> Items = CharacterTileView->GetListItems();
    for (UObject* Item : Items)
    {
        if (auto* RowItem = Cast<UCharacterInfoWrapper>(Item))
        {
            if (RowItem->bTeamSelected == false)
            {
                RowItem->bTeamSelected = (RowItem->DescriptionData.UID == SelectedCharacterID);
            }
        }
    }
}

void UPG_CharacterSelectWidget::RefreshCharacterTileView()
{
    TArray<UUserWidget*> Displayed;
    Displayed = CharacterTileView->GetDisplayedEntryWidgets();

    for (UUserWidget* Widget : Displayed)
    {
        if (UPG_CharacterSelectButton* Btn = Cast<UPG_CharacterSelectButton>(Widget))
        {
            UObject* ListItemObj = Btn->GetListItem();

            if (UCharacterInfoWrapper* Wrap = Cast<UCharacterInfoWrapper>(ListItemObj))
            {
                Btn->ApplySelectedVisual(Wrap);
            }
        }
    }
}

void UPG_CharacterSelectWidget::InitCharacterList()
{
    if (!CharacterTileView || !CharacterDescSubsys)
        return;

    TArray<UObject*> Items;

    const TArray<FName> RowNames = CharacterDescSubsys->GetAllRowNames();

    for (const FName& RowName : RowNames)
    {
        const FCharacterDescription* DescriptionRow = CharacterDescSubsys->GetCharacterDescription(RowName);
        if (!DescriptionRow) continue;
        const FCharacterResourceInfo* ResourceRow = CharacterDescSubsys->GetCharacterResource(RowName);
        if (!ResourceRow) continue;

        UCharacterInfoWrapper* Wrapper = NewObject<UCharacterInfoWrapper>(this);
        Wrapper->RowName = RowName;
        Wrapper->DescriptionData = *DescriptionRow;
        Wrapper->ResourceData = *ResourceRow;
        Items.Add(Wrapper);
    }

    CharacterTileView->SetListItems(Items);
}

bool UPG_CharacterSelectWidget::CheckPlayerState()
{
    if (LobbyPlayerState)
        return true;

    ALobbyPlayerState* FoundPS = Cast<ALobbyPlayerState>(GetOwningPlayerState());
    if (!FoundPS)
        return false;

    LobbyPlayerState = FoundPS;
    int IconIndex = 0;

    SetPlayerNumberID(IconIndex++, LobbyPlayerState->GetPlayerNumberId());

    ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
    for (APlayerState* PS : GS->PlayerArray)
    {
        ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);
        if (!LPS)
        {
            continue;
        }

        if (LPS == LobbyPlayerState)
        {
            continue;
        }

        if (LPS->GetTeamID() != LobbyPlayerState->GetTeamID())
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                    FString::Printf(TEXT("[CLIENT] %d"), LPS->GetTeamID()));
            }
            continue;
        }

        LPS->OnCharacterIDChanged.AddUniqueDynamic(this, &ThisClass::HandleAnyCharacterIDChanged);
        LPS->OnLobbyPlayerStateChangedHelperUI.AddUniqueDynamic(this, &ThisClass::HandlePlayerSelected);

        SetPlayerNumberID(IconIndex++, LPS->GetPlayerNumberId());
    }
    return true;
}


void UPG_CharacterSelectWidget::SetUI()
{
    if (ShowAnim)
    {
        PlayAnimation(ShowAnim);
    }
}

