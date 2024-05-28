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
            ConcreteSetting->SetDefaultValue(FColor::White);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
