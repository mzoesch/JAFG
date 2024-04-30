// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameSetting.h"
#include "GameSettingCollection.h"
#include "UObject/Object.h"

#include "SettingRegistry.generated.h"

JAFG_VOID

class URegistrySubsystem;

UCLASS()
class JAFGCORE_API USettingRegistry : public UObject
{
    GENERATED_BODY()

protected:

    virtual void OnInitialize(void);
    friend URegistrySubsystem;

public:

    UGameSetting* FindSettingByIdentifier(const FString& Identifier);

    template<typename T = UGameSetting>
    T* FindSafeSettingByIdentifier(const FString& Identifier)
    {
        if (T* Setting = Cast<T>(this->FindSettingByIdentifier(Identifier)); Setting)
        {
            return Setting;
        }

        LOG_ERROR(LogGameSettings, "Setting with identifier %s not found.", *Identifier)

        return NewObject<T>();
    }

protected:

    auto RegisterSetting(UGameSetting* InSetting) -> void;
    auto RegisterInnerSetting(UGameSetting* InSetting) -> void;

    auto InitializeGamePlaySettings(void) -> UGameSettingCollection*;
    auto InitializeAudioSettings(void) -> UGameSettingCollection*;
    auto InitializeVideoSettings(void) -> UGameSettingCollection*;
    auto InitializeControlsSettings(void) -> UGameSettingCollection*;
    auto InitializeKeybindingsSettings(void) -> UGameSettingCollection*;
    auto InitializeUserInterfaceSettings(void) -> UGameSettingCollection*;
    auto InitializeDebugSettings(void) -> UGameSettingCollection*;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> TopLevelSettings;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGameSetting>> RegisteredSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> GamePlaySettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> AudioSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> VideoSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> ControlsSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> KeybindingsSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> UserInterfaceSettings;

    UPROPERTY()
    TObjectPtr<UGameSettingCollection> DebugSettings;
};
