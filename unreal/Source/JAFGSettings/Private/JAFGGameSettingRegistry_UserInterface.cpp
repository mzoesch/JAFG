// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingValueColor.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeUserInterfaceSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("UserInterface");
    Screen->SetDisplayName("User Interface");

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("CommonUserInterfaceCollection");
        Subsection->SetDisplayName("Common User Interface");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("PrimaryColorScheme");
            ConcreteSetting->SetDisplayName("Primary Color");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPrimaryColor));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPrimaryColor));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultPrimaryColor);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("PrimaryColorAlphaMaxScheme");
            ConcreteSetting->SetDisplayName("Primary Color Alpha Max");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPrimaryColorAlphaMax));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPrimaryColorAlphaMax));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultPrimaryColorAlphaMax);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("PrimaryColorAlphaMidScheme");
            ConcreteSetting->SetDisplayName("Primary Color Alpha Mid");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPrimaryColorAlphaMid));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPrimaryColorAlphaMid));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultPrimaryColorAlphaMid);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("PrimaryColorAlphaLessScheme");
            ConcreteSetting->SetDisplayName("Primary Color Alpha Less");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPrimaryColorAlphaLess));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPrimaryColorAlphaLess));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultPrimaryColorAlphaLess);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("SecondaryColorScheme");
            ConcreteSetting->SetDisplayName("Secondary Color");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSecondaryColor));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSecondaryColor));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultSecondaryColor);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
