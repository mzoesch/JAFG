// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingRegistry.h"

#include "JAFGLogDefs.h"
#include "SettingsData/GameSetting.h"

UGameSettingRegistry::UGameSettingRegistry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UGameSettingRegistry::Initialize(ULocalPlayer* InLocalPlayer)
{
    if (InLocalPlayer == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Local player is invalid.")
        return;
    }

    this->OwingLocalPlayer = InLocalPlayer;
    this->TopLevelSettings.Empty();
    this->RegisteredSettings.Empty();

    this->OnInitialize();

    return;
}

void UGameSettingRegistry::RegisterTopLevelSetting(UGameSetting* InSetting)
{
    if (InSetting == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Setting is invalid.")
        return;
    }

    this->TopLevelSettings.Add(InSetting);
    InSetting->SetOwningRegistry(this);
    this->RegisterInnerSetting(InSetting);

    return;
}

void UGameSettingRegistry::RegisterInnerSetting(UGameSetting* InSetting)
{
    this->RegisteredSettings.Add(InSetting);

    for (UGameSetting* ChildSetting : InSetting->GetChildSettings())
    {
        this->RegisterInnerSetting(ChildSetting);
    }

    return;
}
