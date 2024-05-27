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
}

void UGameSettingValueScalar::ResetToDefault(void)
{
}

void UGameSettingValueScalar::RestoreToInitial(void)
{
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

void UGameSettingValueScalar::SetValue(double InValue, EGameSettingChangeReason::Type Reason)
{
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

#undef LOCAL_OWNER
