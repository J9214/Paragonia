// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PG_CharacterDescription.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPG_CharacterDescription::NativeOnInitialized()
{
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

    if(CharacterName)
        CharacterName->SetText(CurrentDesc.DisplayName);

    SetSkill1Description();
}

void UPG_CharacterDescription::SetSkill1Description()
{
    if(SkillName)
        SkillName->SetText(CurrentDesc.Skill1Name);
    if(SkillDescription)    
        SkillDescription->SetText(CurrentDesc.Skill1Description);
}

void UPG_CharacterDescription::SetSkill2Description()
{
    if (SkillName)
        SkillName->SetText(CurrentDesc.Skill2Name);
    if (SkillDescription)
        SkillDescription->SetText(CurrentDesc.Skill2Description);
}

void UPG_CharacterDescription::SetSkill3Description()
{
    if (SkillName)
        SkillName->SetText(CurrentDesc.Skill3Name);
    if (SkillDescription)
        SkillDescription->SetText(CurrentDesc.Skill3Description);
}
