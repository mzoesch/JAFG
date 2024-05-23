// Copyright 2024 mzoesch. All rights reserved.

#include "SettingsData/GameSettingRegistry.h"

#include "JAFGLogDefs.h"

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

    return;
}
