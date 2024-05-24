// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingDataSource.h"

#include "JAFGLogDefs.h"

FGameSettingDataSource::FGameSettingDataSource(void)
{
    return;
}

FGameSettingDataSource::FGameSettingDataSource(const TArray<FString>& InDynamicPath) : DynamicPath(InDynamicPath)
{
    return;
}

bool FGameSettingDataSource::Resolve(ULocalPlayer* InLocalPlayer) const
{
    return DynamicPath.Resolve(InLocalPlayer);
}

FString FGameSettingDataSource::GetValueAsString(ULocalPlayer* InLocalPlayer) const
{
    FString OutStringValue;

    if (PropertyPathHelpers::GetPropertyValueAsString(InLocalPlayer, this->DynamicPath, OutStringValue) == false)
    {
        LOG_ERROR(LogGameSettings, "Failed to get property value as string")
        return FString();
    }

    return OutStringValue;
}

void FGameSettingDataSource::SetValue(ULocalPlayer* InLocalPlayer, const FString& InStringValue) const
{
    if (PropertyPathHelpers::SetPropertyValueFromString(InLocalPlayer, this->DynamicPath, InStringValue) == false)
    {
        LOG_ERROR(LogGameSettings, "Failed to set property value from string")
    }
}

FString FGameSettingDataSource::ToString(void) const
{
    return this->DynamicPath.ToString();
}
