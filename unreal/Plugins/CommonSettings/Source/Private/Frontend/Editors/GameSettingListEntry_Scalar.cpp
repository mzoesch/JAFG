// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_Scalar.h"

#include "Components/JAFGSlider.h"
#include "Components/JAFGTextBlock.h"
#include "Foundation/JAFGEnhancedButton.h"
#include "SettingsData/GameSettingValueScalar.h"
#include "Frontend/SettingsTabBarPanel.h"

UGameSettingListEntry_Scalar::UGameSettingListEntry_Scalar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry_Scalar::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Slider_SettingValue->OnValueChanged.AddDynamic(this, &UGameSettingListEntry_Scalar::OnSliderValueChanged);

    this->Button_ResetToDefault->OnClicked().AddUObject(this, &UGameSettingListEntry_Scalar::OnResetToDefaultClicked);

    return;
}

void UGameSettingListEntry_Scalar::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA_YESNO(FGameSettingListEntryPassData_Scalar, false)
    {
        this->Setting = Data->Scalar;
    }

    if (this->Setting->GetDefaultValue().IsSet())
    {
        this->Slider_SettingValue->SetValue(this->Setting->GetDefaultValue().GetValue());
        this->Text_SettingValue->SetText(this->Setting->GetFormattedText());
        this->OnValueChanged(this->Setting->GetDefaultValue().GetValue(), this->Setting->GetValueNormalized());
    }
    else
    {
        LOG_ERROR(
            LogGameSettings,
            "Scalar setting [%s] has no default value set. This is not implemented yet.",
            *this->Setting->GetIdentifier()
        )
    }

    this->UpdateResetToDefaultButton();

    return;
}

void UGameSettingListEntry_Scalar::OnRestoreSettingsToInitial(void)
{
    Super::OnRestoreSettingsToInitial();

    LOG_VERBOSE(
        LogGameSettings,
        "Restoring [%s] scalar setting to initial [%f].",
        *this->Setting->GetIdentifier(), this->Setting->GetInitialValue()
    )

    this->Setting->RestoreToInitial();

    this->Text_SettingValue->SetText(this->Setting->GetFormattedText());
    this->UpdateResetToDefaultButton();
    this->OnValueChanged(this->Setting->GetValue(), this->Setting->GetValueNormalized());

    return;
}

void UGameSettingListEntry_Scalar::OnSliderValueChanged(const float Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_FATAL(LogGameSettings, "Value is not in the range [0, 1]. Found: %f.", Value)
        return;
    }

    this->Setting->SetValue(this->Setting->TransformRangeToValue(Value), EGameSettingChangeReason::Change);

    this->OwningPanel->OnApplyableSettingChanged();

    this->Text_SettingValue->SetText(this->Setting->GetFormattedText());
    this->UpdateResetToDefaultButton();
    this->OnValueChanged(this->Setting->GetValue(), this->Setting->GetValueNormalized());

    return;
}

void UGameSettingListEntry_Scalar::OnResetToDefaultClicked(void)
{
    LOG_DISPLAY(
        LogGameSettings,
        "Resetting [%s] scalar setting to default [%f].",
        *this->Setting->GetIdentifier(), this->Setting->GetDefaultValue().GetValue()
    )

    this->Setting->ResetToDefault();

    this->Text_SettingValue->SetText(this->Setting->GetFormattedText());

    this->OwningPanel->OnApplyableSettingChanged();

    this->UpdateResetToDefaultButton();
    this->OnValueChanged(this->Setting->GetValue(), this->Setting->GetValueNormalized());

    return;
}

void UGameSettingListEntry_Scalar::UpdateResetToDefaultButton(void) const
{
    if (this->Setting->GetDefaultValue().IsSet() == false)
    {
        this->Button_ResetToDefault->SetIsEnabled(false);
        return;
    }

    this->Button_ResetToDefault->SetIsEnabled(this->Setting->GetValue() != this->Setting->GetDefaultValue().GetValue());
    return;
}
