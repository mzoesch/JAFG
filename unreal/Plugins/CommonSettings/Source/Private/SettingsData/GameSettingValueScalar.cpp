// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingValueScalar.h"
#include "JAFGLogDefs.h"

static FText PercentFormat = FText::FromString("{0}%");

UGameSettingValueScalar::UGameSettingValueScalar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingValueScalar::StoreInitial(void)
{
    Super::StoreInitial();
}

void UGameSettingValueScalar::ResetToDefault(void)
{
    if (this->DefaultValue.IsSet() == false)
    {
        LOG_FATAL(LogGameSettings, "Scalar setting [%s] has no default value set.", *this->GetIdentifier())
        return;
    }

    this->SetValue(this->DefaultValue.GetValue(), EGameSettingChangeReason::ResetToDefault);

    return;
}

void UGameSettingValueScalar::RestoreToInitial(void)
{
    Super::RestoreToInitial();
}

FText UGameSettingValueScalar::GetFormattedText(void) const
{
    return this->DisplayFormat(this->GetValue());
}

double UGameSettingValueScalar::GetValue(void) const
{
    const FString OutValue = this->ValueGetter->GetValueAsString(this->OwningPlayer);

    double Value;
    LexFromString(Value, *OutValue);

    return Value;
}

double UGameSettingValueScalar::GetValueNormalized(void)
{
    return FMath::GetMappedRangeValueClamped(this->GetSourceRange(), TRange<double>(0, 1), this->GetValue());
}

void UGameSettingValueScalar::SetValue(double InValue, EGameSettingChangeReason::Type Reason)
{
    InValue = FMath::RoundHalfFromZero(InValue / this->ValueStep);
    InValue = InValue * this->ValueStep;

    if (this->Minimum.IsSet())
    {
        InValue = FMath::Max(this->Minimum.GetValue(), InValue);
    }

    if (this->Maximum.IsSet())
    {
        InValue = FMath::Min(this->Maximum.GetValue(), InValue);
    }

    const FString StringValue = LexToString(InValue);
    this->ValueSetter->SetValue(this->OwningPlayer, StringValue);

    return;
}

float UGameSettingValueScalar::TransformRangeToValue(const float Value)
{
    if (this->Maximum.IsSet() == false || this->Minimum.IsSet() == false)
    {
        return Value;
    }

    const TRange<double> InputRange  = TRange<double>(0, 1);

    return FMath::GetMappedRangeValueClamped(InputRange, this->GetSourceRange(), static_cast<double>(Value));
}

TRange<double> UGameSettingValueScalar::GetSourceRange(void)
{
    const double RangeMin = this->Minimum.IsSet() ? this->Minimum.GetValue() : 0.0;
    const double RangeMax = this->Maximum.IsSet() ? this->Maximum.GetValue() : 1.0;

    return TRange<double>(RangeMin, RangeMax);
}

FSettingScalarFormatFunction UGameSettingValueScalar::Raw( [] (const double Value)
{
    return FText::AsNumber(Value);
});

FSettingScalarFormatFunction UGameSettingValueScalar::ZeroToOneAsPercent( [] (const double Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_ERROR(LogGameSettings, "Value is not in the range [0, 1]. Found: %f.", Value)
        return FText::FromString(TEXT("ERROR"));
    }

    return FText::Format(PercentFormat, static_cast<int32>(FMath::RoundHalfFromZero(100.0 * Value)));
});
