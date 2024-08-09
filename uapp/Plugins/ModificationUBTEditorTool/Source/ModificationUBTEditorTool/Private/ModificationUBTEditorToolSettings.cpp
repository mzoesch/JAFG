// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolSettings.h"

const TCHAR* LexToString(const EModificationUBTEditorToolStartGame::Type InType)
{
    switch (InType)
    {
    case EModificationUBTEditorToolStartGame::Steam:
    {
        return TEXT("Steam");
    }
    case EModificationUBTEditorToolStartGame::SteamServer:
    {
        return TEXT("SteamDS");
    }
    case EModificationUBTEditorToolStartGame::Custom:
    {
        return TEXT("Custom");
    }
    default:
    {
        checkNoEntry()
        return TEXT("");
    }
    }
}

void UModificationUBTEditorToolSettings::SaveSettings(void)
{
    this->SaveConfig();
}

EBuildConfiguration UModificationUBTEditorToolSettings::GetBuildConfiguration() const
{
    return EBuildConfiguration::Shipping;
}

const TMap<FString, FModificationUBTEditorToolTargetSettings> UModificationUBTEditorToolSettings::
GetPlatformTargetSettings() const
{
    return {{TEXT("Windows"), WindowsGameTargetSettings},
                    {TEXT("WindowsServer"), WindowsServerTargetSettings},
                    {TEXT("LinuxServer"), LinuxServerTargetSettings},
                };
}
