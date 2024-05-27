// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SettingsData/GameSettingRegistry.h"

#include "JAFGGameSettingRegistry.generated.h"

class UJAFGLocalPlayer;
class UGameSettingCollection;
class UJAFGGameSettingRegistry;

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)                      \
    MakeShared<FGameSettingDataSource>(TArray<FString>({                              \
        GET_FUNCTION_NAME_STRING_CHECKED(UJAFGLocalPlayer,   GetLocalSettings),       \
        GET_FUNCTION_NAME_STRING_CHECKED(UJAFGSettingsLocal, FunctionOrPropertyName), \
    }))

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
    virtual void OnInitialize(UCustomSettingsLocalPlayer* InOwningPlayer) override;
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
    TObjectPtr<UGameSettingCollection> DeveloperSettings     = nullptr;

    // ~Top Level Settings
    //////////////////////////////////////////////////////////////////////////

    auto InitializeGameplaySettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeAudioSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeVideoSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeControlSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeKeybindingSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeUserInterfaceSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
    auto InitializeDeveloperSettings(UJAFGLocalPlayer* InOwningPlayer) -> UGameSettingCollection*;
};
