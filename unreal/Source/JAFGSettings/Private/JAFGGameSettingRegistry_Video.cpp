// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollections.h"
#include "JAFGLogDefs.h"

bool JAFGGameSettingRegistryHelpers::GetDisplayAdapterScreenResolutions(FScreenResolutionArray& Resolutions, const bool bIgnoreRefreshRate /* = false */)
{
    if (RHIGetAvailableResolutions(Resolutions, bIgnoreRefreshRate))
    {
        LOG_VERBOSE(LogGameSettings, "Obtained %d screen resolutions", Resolutions.Num())
        return true;
    }

    LOG_ERROR(LogGameSettings, "Screen Resolutions could not be obtained")
    return false;
}

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeVideoSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Video");
    Screen->SetDisplayName("Video");

    return Screen;
}
