// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollection.h"
#include "SettingsData/GameSettingValueScalar.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeAudioSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Audio");
    Screen->SetDisplayName("Audio");

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("VolumeCollection");
        Subsection->SetDisplayName("Volume");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MasterVolume");
            ConcreteSetting->SetDisplayName("Master");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMasterVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMasterVolume));
            ConcreteSetting->SetDefaultValue(0.5f);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MusicVolume");
            ConcreteSetting->SetDisplayName("Music");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMasterVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMasterVolume));
            ConcreteSetting->SetDefaultValue(0.75f);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MISCVolume");
            ConcreteSetting->SetDisplayName("MISC");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMasterVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMasterVolume));
            ConcreteSetting->SetDefaultValue(0.25f);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("OtherVolume");
            ConcreteSetting->SetDisplayName("Other");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMasterVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMasterVolume));
            ConcreteSetting->SetDefaultValue(1.0f);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
