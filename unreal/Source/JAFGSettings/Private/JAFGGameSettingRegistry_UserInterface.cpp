// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollection.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeUserInterfaceSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("UserInterface");
    Screen->SetDisplayName("User Interface");

    return Screen;
}
