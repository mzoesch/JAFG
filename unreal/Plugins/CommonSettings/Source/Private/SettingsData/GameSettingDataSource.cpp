// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingDataSource.h"
#include "JAFGLogDefs.h"
#include "CustomSettingsLocalPlayer.h"

FGameSettingDataSource::FGameSettingDataSource(void)
{
    return;
}

FGameSettingDataSource::FGameSettingDataSource(const TArray<FString>& InDynamicPath) : DynamicPath(InDynamicPath)
{
    return;
}

bool FGameSettingDataSource::Resolve(UCustomSettingsLocalPlayer* InLocalPlayer) const
{
    return this->DynamicPath.Resolve(InLocalPlayer);
}

FString FGameSettingDataSource::GetValueAsString(UCustomSettingsLocalPlayer* InLocalPlayer) const
{
    FString OutStringValue;

    if (PropertyPathHelpers::GetPropertyValueAsString(InLocalPlayer, this->DynamicPath, OutStringValue) == false)
    {
        LOG_ERROR(LogGameSettings, "Failed to get property value as string. For path: %s.", *this->ToString())
        return FString();
    }

    return OutStringValue;
}

void FGameSettingDataSource::SetValue(UCustomSettingsLocalPlayer* InLocalPlayer, const FString& InStringValue) const
{
    if (PropertyPathHelpers::SetPropertyValueFromString(InLocalPlayer, this->DynamicPath, InStringValue) == false)
    {
        LOG_ERROR(LogGameSettings, "Failed to set property value from string. For path: %s.", *this->ToString())
    }
}

FString FGameSettingDataSource::ToString(void) const
{
    return this->DynamicPath.ToString();
}
