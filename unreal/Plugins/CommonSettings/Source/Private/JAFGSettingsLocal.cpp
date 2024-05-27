// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGSettingsLocal.h"

UJAFGSettingsLocal::UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

UJAFGSettingsLocal* UJAFGSettingsLocal::Get(void)
{
    return GEngine ? CastChecked<UJAFGSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

float UJAFGSettingsLocal::GetMasterVolume(void) const
{
    return this->MasterVolume;
}

void UJAFGSettingsLocal::SetMasterVolume(const float InMasterVolume)
{
    this->MasterVolume = InMasterVolume;
}
