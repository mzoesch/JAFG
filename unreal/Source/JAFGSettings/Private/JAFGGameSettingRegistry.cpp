// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"

#include "JAFGLogDefs.h"
#include "SettingsData/GameSetting.h"
#include "SettingsData/GameSettingCollection.h"

UJAFGGameSettingRegistrySubsystem::UJAFGGameSettingRegistrySubsystem(void) : Super()
{
    return;
}

void UJAFGGameSettingRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->LocalRegistry = NewObject<UJAFGGameSettingRegistry>(); check( this->LocalRegistry )
    this->LocalRegistry->Initialize(this->GetLocalPlayer());

    LOG_VERBOSE(LogGameSettings, "Initialzied local registry.")

    return;
}

UJAFGGameSettingRegistry::UJAFGGameSettingRegistry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGGameSettingRegistry::OnInitialize(void)
{
    Super::OnInitialize();

    this->GameplaySettings      = this->InitializeGameplaySettings();
    this->AudioSettings         = this->InitializeAudioSettings();
    this->VideoSettings         = this->InitializeVideoSettings();
    this->ControlSettings       = this->InitializeControlSettings();
    this->KeybindingSettings    = this->InitializeKeybindingSettings();
    this->UserInterfaceSettings = this->InitializeUserInterfaceSettings();
    this->DebugSettings         = this->InitializeDebugSettings();

    this->RegisterTopLevelSetting(this->GameplaySettings);
    this->RegisterTopLevelSetting(this->AudioSettings);
    this->RegisterTopLevelSetting(this->VideoSettings);
    this->RegisterTopLevelSetting(this->ControlSettings);
    this->RegisterTopLevelSetting(this->KeybindingSettings);
    this->RegisterTopLevelSetting(this->UserInterfaceSettings);
    this->RegisterTopLevelSetting(this->DebugSettings);

    return;
}
