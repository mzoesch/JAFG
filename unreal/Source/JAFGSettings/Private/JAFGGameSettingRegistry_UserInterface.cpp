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
            ConcreteSetting->SetIdentifier("PrimaryColorAlphaScheme");
            ConcreteSetting->SetDisplayName("Primary Color Alpha");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetPrimaryColorAlpha));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetPrimaryColorAlpha));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultPrimaryColorAlpha);

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

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("MenuUserInterfaceCollection");
        Subsection->SetDisplayName("Menu");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("AddedSubMenuColorScheme");
            ConcreteSetting->SetDisplayName("Added Sub Menu Color");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetAddedSubMenuColor));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetAddedSubMenuColor));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultAddedSubMenuColor);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
