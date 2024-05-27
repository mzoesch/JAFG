// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"

#include "JAFGLogDefs.h"
#include "CustomSettingsLocalPlayer.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSetting.h"
#include "SettingsData/GameSettingCollections.h"

UJAFGGameSettingRegistrySubsystem::UJAFGGameSettingRegistrySubsystem(void) : Super()
{
    return;
}

void UJAFGGameSettingRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->LocalRegistry = NewObject<UJAFGGameSettingRegistry>(); check( this->LocalRegistry )
    this->LocalRegistry->Initialize(this->GetLocalPlayer<UCustomSettingsLocalPlayer>());

    LOG_VERBOSE(LogGameSettings, "Initialzied local registry.")

    return;
}

UJAFGGameSettingRegistry::UJAFGGameSettingRegistry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGGameSettingRegistry::OnInitialize(UCustomSettingsLocalPlayer* InOwningPlayer)
{
    // This subsystem must be initialized last
    Super::OnInitialize(InOwningPlayer);

    UJAFGLocalPlayer* CastedIn = Cast<UJAFGLocalPlayer>(InOwningPlayer);
    if (CastedIn == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Invalid Owning Player received. It is not of type UJAFGLocalPlayer.")
        return;
    }

    this->GameplaySettings      = this->InitializeGameplaySettings(CastedIn);
    this->AudioSettings         = this->InitializeAudioSettings(CastedIn);
    this->VideoSettings         = this->InitializeVideoSettings(CastedIn);
    this->ControlSettings       = this->InitializeControlSettings(CastedIn);
    this->KeybindingSettings    = this->InitializeKeybindingSettings(CastedIn);
    this->UserInterfaceSettings = this->InitializeUserInterfaceSettings(CastedIn);
    this->DeveloperSettings     = this->InitializeDeveloperSettings(CastedIn);

    this->RegisterTopLevelSetting(this->GameplaySettings);
    this->RegisterTopLevelSetting(this->AudioSettings);
    this->RegisterTopLevelSetting(this->VideoSettings);
    this->RegisterTopLevelSetting(this->ControlSettings);
    this->RegisterTopLevelSetting(this->KeybindingSettings);
    this->RegisterTopLevelSetting(this->UserInterfaceSettings);
    this->RegisterTopLevelSetting(this->DeveloperSettings);

    return;
}
