//// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"
#include "Registry/SettingRegistry.h"

UGameSettingCollection* USettingRegistry::InitializeControlsSettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();

    Screen->SetIdentifier("ControlsCollection");
    Screen->SetSettingDisplayName(FText::FromString("Controls"));

    return Screen;
}
