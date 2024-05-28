// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_Color.h"

#include "Components/Border.h"
#include "Foundation/JAFGEnhancedButton.h"
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

    this->Button_ResetToDefault->SetContent(FText::FromString(TEXT("X")));
    this->Button_ResetToDefault->OnClicked().AddUObject(this, &UGameSettingListEntry_Color::OnResetToDefaultClicked);

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
    this->UpdateResetToDefaultButton();

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UGameSettingListEntry_Color::OnTextChanged(const FText& Text)
{
    if (UGameSettingListEntry_Color::IsInTextAViableColor(Text) == false)
    {
        this->OwningPanel->DisallowApply(this->Setting->GetIdentifier());
        this->Border_Container->SetBrushColor(this->DangerContainerColor);
        this->UpdateResetToDefaultButton();
        return;
    }

    this->OwningPanel->ReleaseDisallowApply(this->Setting->GetIdentifier());
    this->Border_Container->SetBrushColor(this->StandardContainerColor);

    const FColor NewUserColor = FColor::FromHex(Text.ToString());
    this->Border_ColorPreview->SetBrushColor(NewUserColor);
    this->Setting->SetValue(NewUserColor);

    this->OwningPanel->OnApplyableSettingChanged();

    this->UpdateResetToDefaultButton();

    return;
}

bool UGameSettingListEntry_Color::IsInTextAViableColor(const FText& Text)
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

void UGameSettingListEntry_Color::OnResetToDefaultClicked(void) const
{
    LOG_DISPLAY(
        LogGameSettings,
        "Resetting [%s] color setting to default [%s].",
        *this->Setting->GetIdentifier(), *this->Setting->GetDefaultValue().ToString()
    )

    this->Setting->ResetToDefault();

    this->SetEditableText(this->Setting->GetValue());
    this->Border_ColorPreview->SetBrushColor(this->Setting->GetValue());

    this->OwningPanel->OnApplyableSettingChanged();

    this->UpdateResetToDefaultButton();

    return;
}

void UGameSettingListEntry_Color::UpdateResetToDefaultButton(void) const
{
    if (this->Setting->GetDefaultValue() == this->Setting->GetValue())
    {
        this->Button_ResetToDefault->SetIsEnabled(false);
        return;
    }

    this->Button_ResetToDefault->SetIsEnabled(true);

    return;
}
