// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolSettings.h"

FString LexToString(const EModificationUBTEditorToolStartGame::Type InType)
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
