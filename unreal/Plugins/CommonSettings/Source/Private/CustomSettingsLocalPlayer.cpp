// Copyright 2024 mzoesch. All rights reserved.

#include "CustomSettingsLocalPlayer.h"

#include "JAFGSettingsLocal.h"

/* Do not make static as we need to access this method through the cached dynamic property paths subsystem. */
// ReSharper disable once CppMemberFunctionMayBeStatic
UJAFGSettingsLocal* UCustomSettingsLocalPlayer::GetLocalSettings(void) const
{
    return UJAFGSettingsLocal::Get();
}
