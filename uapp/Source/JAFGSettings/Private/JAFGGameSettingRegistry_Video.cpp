// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "SettingsData/GameSettingCollections.h"
#include "JAFGLogDefs.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingValueScalar.h"

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

// ReSharper disable once CppMemberFunctionMayBeStatic
UGameSettingCollection* UJAFGGameSettingRegistry::InitializeVideoSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Video");
    Screen->SetDisplayName("Video");

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("WorldGeneration");
        Subsection->SetDisplayName("World Generation");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("ServerRenderDistance");
            ConcreteSetting->SetDisplayName("Server Render Distance");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetServerRenderDistance));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetServerRenderDistance));
            ConcreteSetting->SetUserMinimum(0.0f);
            ConcreteSetting->SetUserMaximum(32.0f);
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultServerRenderDistance));

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("ClientRenderDistance");
            ConcreteSetting->SetDisplayName("Client Render Distance");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetClientRenderDistance));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetClientRenderDistance));
            ConcreteSetting->SetUserMinimum(0.0f);
            ConcreteSetting->SetUserMaximum(32.0f);
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultClientRenderDistance));

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
