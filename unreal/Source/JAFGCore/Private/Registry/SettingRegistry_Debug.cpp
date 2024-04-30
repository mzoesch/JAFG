// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"
#include "Registry/SettingRegistry.h"

UGameSettingCollection* USettingRegistry::InitializeDebugSettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();

    Screen->SetIdentifier("DebugCollection");
    Screen->SetSettingDisplayName(FText::FromString("Debug"));

    return Screen;
}
