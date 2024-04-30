// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"
#include "Registry/SettingRegistry.h"

UGameSettingCollection* USettingRegistry::InitializeUserInterfaceSettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();

    Screen->SetIdentifier("UserInterfaceCollection");
    Screen->SetSettingDisplayName(FText::FromString("User Interface"));

    return Screen;
}
