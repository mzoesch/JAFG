// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"
#include "Registry/SettingRegistry.h"

UGameSettingCollection* USettingRegistry::InitializeKeybindingsSettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();

    Screen->SetIdentifier("KAndMCollection");
    Screen->SetSettingDisplayName(FText::FromString("Mouse & Keyboard"));

    return Screen;
}
