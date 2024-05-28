// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_Color.h"

#include "Components/Border.h"
#include "Input/JAFGEditableText.h"
#include "SettingsData/GameSettingValueColor.h"

UGameSettingListEntry_Color::UGameSettingListEntry_Color(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry_Color::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->EditableText_SettingValue->OnTextChanged.AddDynamic(this, &UGameSettingListEntry_Color::OnTextChanged);

    return;
}

void UGameSettingListEntry_Color::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FGameSettingListEntryPassData_Color)
    {
        this->Setting = Data->Color;
    }

    if (Setting == nullptr || !Setting->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid Setting received.")
        return;
    }

    return;
}

void UGameSettingListEntry_Color::OnTextChanged(const FText& Text)
{
    if (Text.ToString().Len() != 9)
    {
        LOG_WARNING(LogCommonSlate, "Invalid color format. Expected: #RRGGBBAA. Found: %s", *Text.ToString())
        return;
    }

    FColor Color = FColor::FromHex(Text.ToString());

    this->Border_ColorPreview->SetBrushColor(Color);

    return;
}
