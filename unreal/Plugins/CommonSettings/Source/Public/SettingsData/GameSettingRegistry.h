// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "GameSettingRegistry.generated.h"

class UGameSetting;

/**
 * Unique to one local player. Should store all settings for this player.
 * Handles changes, saving, and loading of settings, as well as an integration with widgets.
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UGameSettingRegistry : public UObject
{
    GENERATED_BODY()

    friend UGameSetting;

public:

    explicit UGameSettingRegistry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void Initialize(ULocalPlayer* InLocalPlayer);

protected:

    virtual void OnInitialize(void) { };

    auto RegisterTopLevelSetting(UGameSetting* InSetting) -> void;
    auto RegisterInnerSetting(UGameSetting* InSetting) -> void;

    UPROPERTY()
    TObjectPtr<ULocalPlayer> OwingLocalPlayer;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> TopLevelSettings;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> RegisteredSettings;
};
