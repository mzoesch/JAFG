// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollection.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeDebugSettings(void)
{
    return NewObject<UGameSettingCollection>();
}
