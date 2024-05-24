// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGSettingsLocal.h"

UJAFGSettingsLocal::UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

UJAFGSettingsLocal* UJAFGSettingsLocal::Get()
{
    return GEngine ? CastChecked<UJAFGSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

float UJAFGSettingsLocal::GetMasterVolume() const
{
    return this->MasterVolume;
}

void UJAFGSettingsLocal::SetMasterVolume(float InMasterVolume)
{
    this->MasterVolume;
}
