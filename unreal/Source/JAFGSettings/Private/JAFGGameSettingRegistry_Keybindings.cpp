// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollection.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeKeybindingSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Keybindings");
    Screen->SetDisplayName("Keybindings");

    return Screen;
}
