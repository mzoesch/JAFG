// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"

#include "GameSettingValue.generated.h"

/**
 * The base class (interface) for all settings that are conceptually a value, that can be
 * changed, and thus reset or restored to their initial value.
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UGameSettingValue : public UGameSetting
{
    GENERATED_BODY()

public:

    explicit UGameSettingValue(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Stores an initial value for the setting.
     * This will be called OnInitialized, but should also be called if the setting is "applied".
     */
    virtual auto StoreInitial(void) -> void PURE_VIRTUAL(UGameSettingValue::StoreInitial)

    /** Resets the property to the default. */
    virtual auto ResetToDefault(void) -> void PURE_VIRTUAL(UGameSettingValue::ResetToDefault)

    /**
     * Restores the setting to the initial value, this is the value when the settings menu is
     * opened before any tweaks were made.
     */
    virtual auto RestoreToInitial(void) -> void PURE_VIRTUAL(UGameSettingValue::RestoreToInitial)

protected:

    // UGameSetting implementation
    virtual auto OnInitialized(void) -> void override;
    virtual auto OnApply(void) -> void;
    // ~UGameSetting implementation
};
