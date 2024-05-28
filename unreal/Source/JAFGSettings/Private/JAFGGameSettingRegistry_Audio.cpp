// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollections.h"
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
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultMasterVolume);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MusicVolume");
            ConcreteSetting->SetDisplayName("Music");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMusicVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMiscVolume));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultMusicVolume);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MiscVolume");
            ConcreteSetting->SetDisplayName("Misc");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMiscVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMiscVolume));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultMiscVolume);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("VoiceVolume");
            ConcreteSetting->SetDisplayName("Voice");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetVoiceVolume));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetVoiceVolume));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultVoiceVolume);

            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
