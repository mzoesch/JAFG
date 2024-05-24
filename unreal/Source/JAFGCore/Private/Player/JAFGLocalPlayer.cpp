// Copyright 2024 mzoesch. All rights reserved.

#include "Player/JAFGLocalPlayer.h"

#include "JAFGSettingsLocal.h"

UJAFGSettingsLocal* UJAFGLocalPlayer::GetLocalSettings() const
{
    return UJAFGSettingsLocal::Get();
}
