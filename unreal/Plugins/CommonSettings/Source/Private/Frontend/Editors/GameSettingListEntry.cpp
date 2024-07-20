// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry.h"

#include "Components/JAFGTextBlock.h"
#include "Frontend/SettingsTabBarPanel.h"

UGameSettingListEntry::UGameSettingListEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FGameSettingListEntryPassData)
    {
        this->Text_SettingName->SetText(Data->SettingName);
        this->OwningPanel = Data->OwningPanel;
    }

    if (this->OwningPanel == nullptr || !this->OwningPanel->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid Owning Panel received.")
        return;
    }

    this->OwningPanel->OnRestoreSettingsToInitialDelegate.AddLambda( [this] (void) { this->OnRestoreSettingsToInitial(); });

    return;
}
