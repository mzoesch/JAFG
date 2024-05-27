// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/Editors/GameSettingListEntry_Scalar.h"

#include "Components/JAFGSlider.h"
#include "Components/JAFGTextBlock.h"
#include "SettingsData/GameSettingValueScalar.h"

UGameSettingListEntry_Scalar::UGameSettingListEntry_Scalar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingListEntry_Scalar::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Slider_SettingValue->OnValueChanged.AddDynamic(this, &UGameSettingListEntry_Scalar::OnSliderValueChanged);

    return;
}

void UGameSettingListEntry_Scalar::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    Super::PassDataToWidget(UncastedData);

    CAST_PASSED_DATA(FGameSettingListEntryPassData_Scalar)
    {
        this->Setting = Data->Scalar;
    }

    if (this->Setting->GetDefaultValue().IsSet())
    {
        this->Slider_SettingValue->SetValue(this->Setting->GetDefaultValue().GetValue());
        this->Text_SettingValue->SetText(this->Setting->GetFormattedText());
        this->OnValueChanged(this->Setting->GetDefaultValue().GetValue());
    }

    return;
}

void UGameSettingListEntry_Scalar::OnSliderValueChanged(const float Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_FATAL(LogGameSettings, "Value is not in the range [0, 1]. Found: %f.", Value)
        return;
    }

    this->Setting->SetValue(Value, EGameSettingChangeReason::Change);

    this->Text_SettingValue->SetText(this->Setting->GetFormattedText());

    this->OnValueChanged(Value); //this->Setting->GetValue());

    return;
}
