// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry.h"

#include "Components/JAFGTextBlock.h"

UGameSettingListEntry::UGameSettingListEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FGameSettingListEntryPassData)
    {
        this->Text_SettingName->SetText(Data->SettingName);
    }

    return;
}
