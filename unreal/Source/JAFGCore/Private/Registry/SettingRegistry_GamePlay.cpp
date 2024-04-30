// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/GameSettingCollection.h"
#include "Registry/SettingRegistry.h"

UGameSettingCollection* USettingRegistry::InitializeGamePlaySettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();

    Screen->SetIdentifier("GamePlayCollection");
    Screen->SetSettingDisplayName(FText::FromString("GamePlay"));

    return Screen;
}
