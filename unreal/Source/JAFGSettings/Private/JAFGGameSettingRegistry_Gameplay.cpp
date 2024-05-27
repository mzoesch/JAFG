// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingValueScalar.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeGameplaySettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Gameplay");
    Screen->SetDisplayName("Gameplay");

    // Language
    // Network Quality
    // Headbobbing
    // camera movement feedback strength
    // autosave interfva
    // autosave notification
    // break notifications
    // mouse smoothing

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
