// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_KeyIn.h"

#include "Foundation/JAFGEnhancedButton.h"
#include "SettingsData/GameSettingValueKeyIn.h"

UGameSettingListEntry_KeyIn::UGameSettingListEntry_KeyIn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry_KeyIn::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Button_PrimaryKey->OnClicked().AddUObject(this, &UGameSettingListEntry_KeyIn::OnPrimaryKeyClicked);
    this->Button_SecondaryKey->OnClicked().AddUObject(this, &UGameSettingListEntry_KeyIn::OnSecondaryKeyClicked);
    this->Button_Clear->OnClicked().AddUObject(this, &UGameSettingListEntry_KeyIn::OnClearClicked);

    return;
}

void UGameSettingListEntry_KeyIn::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FGameSettingListEntryPassData_KeyIn)
    {
        this->Setting = Data->KeyIn;
    }

    if (this->Setting == nullptr || !this->Setting->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid Setting received.")
        return;
    }

    this->Button_Clear->SetContent(FText::FromString("X"));

    this->SetAllMutableTexts();

    return;
}

void UGameSettingListEntry_KeyIn::OnPrimaryKeyClicked(void)
{
    LOG_WARNING(LogTemp, "UGameSettingListEntry_KeyIn::OnPrimaryKeyClicked not implemented.")
}

void UGameSettingListEntry_KeyIn::OnSecondaryKeyClicked(void)
{
    LOG_WARNING(LogTemp, "UGameSettingListEntry_KeyIn::OnPrimaryKeyClicked not implemented.")
}

void UGameSettingListEntry_KeyIn::OnClearClicked(void)
{
    LOG_WARNING(LogTemp, "UGameSettingListEntry_KeyIn::OnPrimaryKeyClicked not implemented.")
}

void UGameSettingListEntry_KeyIn::SetAllMutableTexts(void) const
{
    this->Button_PrimaryKey->SetContent(this->Setting->GetPrimaryKeyText());
    this->Button_SecondaryKey->SetContent(this->Setting->GetSecondaryKeyText());

    return;
}
