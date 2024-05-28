// Copyright 2024 mzoesch. All rights reserved.

#include "CommonSettingsDefaultColorSubsystem.h"

#include "JAFGSettingsLocal.h"

void UCommonSettingsDefaultColorSubsystem::GiveSelfToSettings(void)
{
    UJAFGSettingsLocal::Get()->SetAndUpdateDefaultColorsSubsystem(this);
}
