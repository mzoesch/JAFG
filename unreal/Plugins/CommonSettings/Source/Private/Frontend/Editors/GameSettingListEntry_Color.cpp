// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_Color.h"

#include "Components/Border.h"
#include "Frontend/SettingsTabBarPanel.h"
#include "Input/JAFGEditableText.h"
#include "SettingsData/GameSettingValueColor.h"

UGameSettingListEntry_Color::UGameSettingListEntry_Color(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry_Color::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->EditableText_SettingValue->SetMaxSize(9 /* Hex: #RRGGBBAA */);
    this->EditableText_SettingValue->OnTrimmedTextChanged.AddUObject(this, &UGameSettingListEntry_Color::OnTextChanged);

    this->StandardContainerColor = this->Border_Container->GetBrushColor().ToFColor(false);

    return;
}

void UGameSettingListEntry_Color::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FGameSettingListEntryPassData_Color)
    {
        this->Setting = Data->Color;
    }

    if (this->Setting == nullptr || !this->Setting->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Invalid Setting received.")
        return;
    }

    const FColor CurrentColor = this->Setting->GetValue();

    this->SetEditableText(CurrentColor);
    this->Border_ColorPreview->SetBrushColor(CurrentColor);

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UGameSettingListEntry_Color::OnTextChanged(const FText& Text)
{
    if (this->IsInTextAViableColor(Text) == false)
    {
        this->OwningPanel->DisallowApply(this->Setting->GetIdentifier());
        this->Border_Container->SetBrushColor(this->DangerContainerColor);
        return;
    }

    this->OwningPanel->ReleaseDisallowApply(this->Setting->GetIdentifier());
    this->Border_Container->SetBrushColor(this->StandardContainerColor);
    const FColor NewUserColor = FColor::FromHex(Text.ToString());
    this->Border_ColorPreview->SetBrushColor(NewUserColor);
    this->Setting->SetValue(NewUserColor);
    this->OwningPanel->OnApplyableSettingChanged();

    return;
}

bool UGameSettingListEntry_Color::IsInTextAViableColor(const FText& Text) const
{
    if (Text.ToString().Len() != 9)
    {
        return false;
    }

    FString AsString = Text.ToString();

    if (AsString[0] != TEXT('#'))
    {
        return false;
    }

    FString Hex = AsString.RightChop(1);

    if (Hex.Len() != 8)
    {
        return false;
    }

    for (const TCHAR& Char : Hex)
    {
        if (FChar::IsHexDigit(Char) == false)
        {
            return false;
        }
    }

    return true;
}

void UGameSettingListEntry_Color::SetEditableText(const FColor& Color) const
{
    this->EditableText_SettingValue->SetText(FText::FromString(FString::Printf(TEXT("#%s"), *Color.ToHex())));
}
