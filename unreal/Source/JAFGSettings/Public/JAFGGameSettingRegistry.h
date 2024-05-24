// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SettingsData/GameSettingRegistry.h"

#include "JAFGGameSettingRegistry.generated.h"

class UGameSettingCollection;
class UJAFGGameSettingRegistry;

UCLASS(NotBlueprintable)
class UJAFGGameSettingRegistrySubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    UJAFGGameSettingRegistrySubsystem();

    // ULocalPlayerSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~ULocalPlayerSubsystem implementation

    UPROPERTY()
    TObjectPtr<UJAFGGameSettingRegistry> LocalRegistry;
};

UCLASS(NotBlueprintable)
class JAFGSETTINGS_API UJAFGGameSettingRegistry : public UGameSettingRegistry
{
    GENERATED_BODY()

public:

    explicit UJAFGGameSettingRegistry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UGameSettingRegistry implementation
    virtual void OnInitialize(void) override;
    // ~UGameSettingRegistry implementation

    //////////////////////////////////////////////////////////////////////////
    // Top Level Settings

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> GameplaySettings      = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> AudioSettings         = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> VideoSettings         = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> ControlSettings       = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> KeybindingSettings    = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> UserInterfaceSettings = nullptr;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> DebugSettings         = nullptr;

    // ~Top Level Settings
    //////////////////////////////////////////////////////////////////////////

    auto InitializeGameplaySettings(void) -> UGameSettingCollection*;
    auto InitializeAudioSettings(void) -> UGameSettingCollection*;
    auto InitializeVideoSettings(void) -> UGameSettingCollection*;
    auto InitializeControlSettings(void) -> UGameSettingCollection*;
    auto InitializeKeybindingSettings(void) -> UGameSettingCollection*;
    auto InitializeUserInterfaceSettings(void) -> UGameSettingCollection*;
    auto InitializeDebugSettings(void) -> UGameSettingCollection*;
};
