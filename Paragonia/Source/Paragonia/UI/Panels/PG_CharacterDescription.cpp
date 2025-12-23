// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Panels/PG_CharacterDescription.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Subsystem/PGStringTableSubsystem.h"
#include "UI/Stat/PG_StatInfo.h"

void UPG_CharacterDescription::NativeOnInitialized()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        StringTableSubsys = GI->GetSubsystem<UPGStringTableSubsystem>();
    }

    if (!StringTableSubsys)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterDescSubsys is null (PGStringTableSubsystem)."));
        return;
    }

    if (Skill1)
    {
        Skill1->OnClicked.AddDynamic(
            this, &ThisClass::SetSkill1Description);
    }
    if (Skill2)
    {
        Skill2->OnClicked.AddDynamic(
            this, &ThisClass::SetSkill2Description);
    }
    if (Skill3)
    {
        Skill3->OnClicked.AddDynamic(
            this, &ThisClass::SetSkill3Description);
    }

}

void UPG_CharacterDescription::InitDescription(const FCharacterDescription& InDesc)
{
    CurrentDesc = InDesc;

    if (CharacterName)
        CharacterName->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.DisplayName));

    for (auto Stat : StatWidgetMap)
    {
        Stat.Value->SetVisibility(ESlateVisibility::Hidden);
    }

    SetSkill1Description();

}

void UPG_CharacterDescription::SetStatInfo(const FName& StatName, float StatValue)
{
    auto FindStatWidget = StatWidgetMap.Find(StatName);

    if (!FindStatWidget)
    {
        UPG_StatInfo* NewWidget = CreateWidget<UPG_StatInfo>(GetOwningPlayer(), StatInfoClass);
        if (!NewWidget)
        {
            return;
        }

        NewWidget->SetVisibility(ESlateVisibility::Visible);
        if (NewWidget->SetStat(StatName, StatValue))
        {
            StatWrapBox->AddChildToWrapBox(NewWidget);
            StatWidgetMap.Add(StatName, NewWidget);
        }
    }
    else
    {
        if (FindStatWidget->Get())
        {
            FindStatWidget->Get()->SetVisibility(ESlateVisibility::Visible);
            FindStatWidget->Get()->ChangeValue(StatValue);
        }
    }
}

void UPG_CharacterDescription::SetSkill1Description()
{
    if (SkillName)
    {
        SkillName->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill1Name));
    }
    if (SkillDescription)
    {
        SkillDescription->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill1Description));
    }
}

void UPG_CharacterDescription::SetSkill2Description()
{
    if (SkillName)
    {
        SkillName->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill2Name));
    }
    if (SkillDescription)
    {
        SkillDescription->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill2Description));
    }
}

void UPG_CharacterDescription::SetSkill3Description()
{
    if (SkillName)
    {
        SkillName->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill3Name));
    }
    if (SkillDescription)
    {
        SkillDescription->SetText(StringTableSubsys->GetText(ELanguageType::English, CurrentDesc.Skill3Description));
    }
}
