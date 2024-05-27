// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollections.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeDeveloperSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Developer");
    Screen->SetDisplayName("Developer");

    return Screen;
}
