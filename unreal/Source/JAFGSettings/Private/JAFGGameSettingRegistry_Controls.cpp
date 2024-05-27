// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollection.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeControlSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Controls");
    Screen->SetDisplayName("Controls");

    return Screen;
}
