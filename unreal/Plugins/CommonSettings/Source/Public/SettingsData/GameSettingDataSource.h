// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "PropertyPathHelpers.h"

class UCustomSettingsLocalPlayer;

class COMMONSETTINGS_API FGameSettingDataSource final : public TSharedFromThis<FGameSettingDataSource>
{

public:

    /** Default constructor. Required by shared refs. Do not use. */
    FGameSettingDataSource(void);

    explicit FGameSettingDataSource(const TArray<FString>& InDynamicPath);
    ~FGameSettingDataSource(void) { }

    auto Resolve(UCustomSettingsLocalPlayer* InLocalPlayer) const -> bool;
    auto GetValueAsString(UCustomSettingsLocalPlayer* InLocalPlayer) const -> FString;
    auto SetValue(UCustomSettingsLocalPlayer* InLocalPlayer, const FString& InStringValue) const -> void;
    auto ToString(void) const -> FString;

private:

    FCachedPropertyPath DynamicPath;
};
