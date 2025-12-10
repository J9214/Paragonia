// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_TitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CommonButtonBase.h"
#include "Components/CanvasPanelSlot.h"

void UPG_TitleWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GameStartButton)
    {
        GameStartButton->OnHovered.AddDynamic(
            this, &ThisClass::HandleGameStartHovered);

        GameStartButton->OnUnhovered.AddDynamic(
            this, &ThisClass::HandleGameStartUnhovered);

        GameStartButton->OnClicked.AddDynamic(
            this, &ThisClass::HandleGameStartClicked);
    }

    if (MatchingInfoButton)
    {
        MatchingInfoButton->OnHovered().AddUObject(
            this, &ThisClass::HandleMatchingInfoHovered);

        MatchingInfoButton->OnUnhovered().AddUObject(
            this, &ThisClass::HandleMatchingInfoUnhovered);

        MatchingInfoButton->OnClicked().AddUObject(
            this, &ThisClass::HandleMatchingInfoCancelRequested);
    }

    if (GameStartButtonText)
    {
        GameStartButtonText->SetText(FText::FromString(TEXT("GameStart")));
    }

    ReadyState = ETitleReadyState::Closed;
    bIsMoving = false;

    ApplyPosition(MatchingInfoClosedPos);
}

void UPG_TitleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bIsMoving || !MatchingInfoButton)
        return;

    MoveElapsed += InDeltaTime;
    float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.f, 1.f);

    FVector2D NewPos = FMath::Lerp(MoveStartPos, MoveTargetPos, Alpha);
    ApplyPosition(NewPos);

    if (Alpha >= 1.f)
    {
        bIsMoving = false;
        ApplyPosition(MoveTargetPos);
    }
}

//PlayerController에 있는 매칭 상태 데이터에 맞춰서 이 후 동작 변경
void UPG_TitleWidget::HandleGameStartClicked()
{
    switch (ReadyState)
    {
    case ETitleReadyState::Closed:
        SetReadyState(ETitleReadyState::InfoOnly);
        if (ModeSelectButton)
            ModeSelectButton->SetIsEnabled(false);
        break;
    case ETitleReadyState::InfoOnly:
    case ETitleReadyState::Armed:
        SetReadyState(ETitleReadyState::Closed);
        if (ModeSelectButton)
            ModeSelectButton->SetIsEnabled(true);
        if (GameStartButtonText)
            GameStartButtonText->SetText(FText::FromString(TEXT("GameStart")));
        break;
    default:
        break;
    }

}

void UPG_TitleWidget::HandleGameStartHovered()
{
    if (ReadyState == ETitleReadyState::Closed)
        return;

    if (GameStartButtonText)
        GameStartButtonText->SetText(FText::FromString(TEXT("Cancel")));
}

void UPG_TitleWidget::HandleGameStartUnhovered()
{
    if (ReadyState == ETitleReadyState::Closed)
        return;

    if (GameStartButtonText)
        GameStartButtonText->SetText(FText::FromString(TEXT("Maching")));
}

void UPG_TitleWidget::HandleMatchingInfoHovered()
{
    if (ReadyState == ETitleReadyState::Closed)
        return;

    SetReadyState(ETitleReadyState::Armed);
}

void UPG_TitleWidget::HandleMatchingInfoUnhovered()
{
    if (ReadyState == ETitleReadyState::Closed)
        return;

    SetReadyState(ETitleReadyState::InfoOnly);
}

void UPG_TitleWidget::HandleMatchingInfoCancelRequested()
{
    SetReadyState(ETitleReadyState::Closed);

    if (ModeSelectButton)
        ModeSelectButton->SetIsEnabled(true);

    if (GameStartButtonText)
        GameStartButtonText->SetText(FText::FromString(TEXT("GameStart")));
}

void UPG_TitleWidget::StartMoveTo(const FVector2D& TargetPos)
{
    if (!MatchingInfoButton)
        return;

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MatchingInfoButton->Slot))
    {
        MoveStartPos = CanvasSlot->GetPosition();
        MoveTargetPos = TargetPos;
        MoveElapsed = 0.f;
        bIsMoving = true;
    }
}

void UPG_TitleWidget::ApplyPosition(const FVector2D& NewPos)
{
    if (!MatchingInfoButton)
        return;

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MatchingInfoButton->Slot))
    {
        CanvasSlot->SetPosition(NewPos);
    }
}

void UPG_TitleWidget::SetReadyState(ETitleReadyState NewState)
{
    if (ReadyState == NewState)
        return;

    ReadyState = NewState;

    switch (ReadyState)
    {
    case ETitleReadyState::Closed:
        StartMoveTo(MatchingInfoClosedPos);
        break;

    case ETitleReadyState::InfoOnly:
        StartMoveTo(MatchingInfoInfoOnlyPos);
        break;

    case ETitleReadyState::Armed:
        StartMoveTo(MatchingInfoArmedPos);
        break;
    }
}
