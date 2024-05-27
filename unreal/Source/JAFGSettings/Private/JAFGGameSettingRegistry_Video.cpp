// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollections.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeVideoSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Video");
    Screen->SetDisplayName("Video");

    return Screen;
}
