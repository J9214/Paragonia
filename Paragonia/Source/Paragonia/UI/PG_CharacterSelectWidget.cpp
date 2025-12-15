// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterSelectWidget.h"
#include "CommonTileView.h"
#include "UI/PG_CharacterDescription.h"
#include "Components/CanvasPanelSlot.h"
#include "Subsystem/PGCharacterDescriptionSubsystem.h"
#include "Engine/GameInstance.h"
#include "Struct/CharacterDescriptionWrapper.h"
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

void UPG_CharacterSelectWidget::SetPlayerCharacterIcon(int index, int CharacterUID)
{
    switch (index)
    {
    case 0:
        Player0Icon->SetPlayerIcon(CharacterUID);
        break;
    case 1:
        Player1Icon->SetPlayerIcon(CharacterUID);
        break;
    case 2:
        Player2Icon->SetPlayerIcon(CharacterUID);
        break;
    default:
        break;
    }
}

void UPG_CharacterSelectWidget::SetPlayerReady(int index)
{
    switch (index)
    {
    case 0:
        Player0Icon->ApplyIcon();
        break;
    case 1:
        Player1Icon->ApplyIcon();
        break;
    case 2:
        Player2Icon->ApplyIcon();
        break;
    default:
        break;
    }
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

    UCharacterDescriptionWrapper* Entry = Cast<UCharacterDescriptionWrapper>(Item);
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
    for (UObject* Item : Items)
    {
        if (UCharacterDescriptionWrapper* RowItem = Cast<UCharacterDescriptionWrapper>(Item))
        {
            int RowUID = RowItem->Data.UID;
            RowItem->bSelected = (RowUID == Entry->Data.UID);
        }
    }

    RefreshCharacterTileView();

    if (CheckPlayerState())
    {
        LobbyPlayerState->ServerSetCharacterID(Entry->Data.UID);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("PlayerState is not ready yet!"));

    CharacterDescription->InitDescription(*DetailRow);
    SelectedCharacterUID = Entry->Data.UID;

    if (SpawnPreviewActorIfNeeded())
    {
        USkeletalMesh* Mesh = Entry->Data.LobbyMesh.LoadSynchronous();
        UAnimMontage* IntroMontage = Entry->Data.IntroMontage.LoadSynchronous();
        UClass* ABP = Entry->Data.LobbyAnimBP.LoadSynchronous();

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
        if (auto* RowItem = Cast<UCharacterDescriptionWrapper>(Item))
        {
            RowItem->bPlayerSelected = (RowItem->Data.UID == SelectedCharacterUID);
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
    if (Player1Icon)
    {
        Player1Icon->SetPlayerIcon(NewCharacterID);
    }

    RefreshCharacterTileView();
}

void UPG_CharacterSelectWidget::HandlePlayerSelected(EPlayerLobbyState LobbyState)
{
    if (LobbyState != EPlayerLobbyState::PLS_SelectedAndReady)
        return;

    TArray<UObject*> Items = CharacterTileView->GetListItems();

    for (UObject* Item : Items)
    {
        //if (auto* RowItem = Cast<UCharacterDescriptionWrapper>(Item))
            //RowItem->bTeamSelected = (RowItem->Data.UID == NewCharacterID);
    }

    if (Player1Icon)
    {
        Player1Icon->ApplyIcon();
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

            if (UCharacterDescriptionWrapper* Wrap = Cast<UCharacterDescriptionWrapper>(ListItemObj))
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
        const FCharacterDescription* Row = CharacterDescSubsys->GetCharacterDescription(RowName);
        if (!Row) continue;

        UCharacterDescriptionWrapper* Wrapper = NewObject<UCharacterDescriptionWrapper>(this);
        Wrapper->RowName = RowName;
        Wrapper->Data = *Row; 
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

    ALobbyGameStateBase* GS = GetWorld()->GetGameState<ALobbyGameStateBase>();
    for (APlayerState* PS : GS->PlayerArray)
    {
        ALobbyPlayerState* LPS = Cast<ALobbyPlayerState>(PS);
        if (!LPS || LPS == LobbyPlayerState) continue;

        LPS->OnCharacterIDChanged.AddUniqueDynamic(this, &ThisClass::HandleAnyCharacterIDChanged);
        LPS->OnLobbyPlayerStateChanged.AddUniqueDynamic(this, &ThisClass::HandlePlayerSelected);
        //OnLobbyPlayerStateChanged에 어떤 캐릭터 선택했는지도 알수 있었으면 합니다 ㅠ
        //PlayerState 전부 순회하면서 선택된거 for문돌리면 기능은 구현 가능한데 많이 과한거 같아서요... 
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

