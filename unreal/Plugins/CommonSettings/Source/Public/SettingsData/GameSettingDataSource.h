// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "PropertyPathHelpers.h"

class COMMONSETTINGS_API FGameSettingDataSource final : public TSharedFromThis<FGameSettingDataSource>
{

public:

    /** Default constructor. Required by shared refs. Do not use. */
    FGameSettingDataSource(void);

    explicit FGameSettingDataSource(const TArray<FString>& InDynamicPath);
    ~FGameSettingDataSource(void) { }

    auto Resolve(ULocalPlayer* InLocalPlayer) const -> bool;
    auto GetValueAsString(ULocalPlayer* InLocalPlayer) const -> FString;
    auto SetValue(ULocalPlayer* InLocalPlayer, const FString& InStringValue) const -> void;
    auto ToString(void) const -> FString;

private:

    FCachedPropertyPath DynamicPath;
};
