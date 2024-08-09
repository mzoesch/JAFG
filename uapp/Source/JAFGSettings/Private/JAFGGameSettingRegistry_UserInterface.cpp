// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingValueColor.h"
#include "SettingsData/GameSettingValueScalar.h"

// ReSharper disable once CppMemberFunctionMayBeStatic
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

        {
            UGameSettingValueColor* ConcreteSetting = NewObject<UGameSettingValueColor>();
            ConcreteSetting->SetIdentifier("InGameOSDColorScheme");
            ConcreteSetting->SetDisplayName("In Game OSD Color");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetInGameOSDColor));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetInGameOSDColor));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultInGameOSDColor);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("HeavyBlurStrengthScheme");
            ConcreteSetting->SetDisplayName("Heavy Blur Strength");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetHeavyBlurStrength));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetHeavyBlurStrength));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultHeavyBlurStrength);

            ConcreteSetting->SetUserMinimum(0.0f);
            ConcreteSetting->SetUserMaximum(20.0f);
            ConcreteSetting->SetValueStep(0.1f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MediumBlurStrengthScheme");
            ConcreteSetting->SetDisplayName("Medium Blur Strength");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetMediumBlurStrength));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetMediumBlurStrength));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultMediumBlurStrength);

            ConcreteSetting->SetUserMinimum(0.0f);
            ConcreteSetting->SetUserMaximum(20.0f);
            ConcreteSetting->SetValueStep(0.1f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("LightBlurStrengthScheme");
            ConcreteSetting->SetDisplayName("Light Blur Strength");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetLightBlurStrength));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetLightBlurStrength));
            ConcreteSetting->SetDefaultValue(UJAFGSettingsLocal::DefaultLightBlurStrength);

            ConcreteSetting->SetUserMinimum(0.0f);
            ConcreteSetting->SetUserMaximum(20.0f);
            ConcreteSetting->SetValueStep(0.1f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("FontUserInterfaceCollection");
        Subsection->SetDisplayName("Fonts");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("GargantuanHeaderFontSizeScheme");
            ConcreteSetting->SetDisplayName("Gargantuan Header Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetGargantuanHeaderFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetGargantuanHeaderFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultGargantuanHeaderFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("HeaderFontSizeScheme");
            ConcreteSetting->SetDisplayName("Header Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetHeaderFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetHeaderFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultHeaderFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("SubHeaderFontSizeScheme");
            ConcreteSetting->SetDisplayName("Sub Header Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSubHeaderFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSubHeaderFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultSubHeaderFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("BodyFontSizeScheme");
            ConcreteSetting->SetDisplayName("Body Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetBodyFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetBodyFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultBodyFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("SmallFontSizeScheme");
            ConcreteSetting->SetDisplayName("Small Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSmallFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSmallFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultSmallFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("TinyFontSizeScheme");
            ConcreteSetting->SetDisplayName("Tiny Font Size");

            ConcreteSetting->SetValueGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetTinyFontSize));
            ConcreteSetting->SetValueSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetTinyFontSize));
            ConcreteSetting->SetDefaultValue(static_cast<float>(UJAFGSettingsLocal::DefaultTinyFontSize));

            ConcreteSetting->SetUserMinimum(5.0f);
            ConcreteSetting->SetUserMaximum(60.0f);
            ConcreteSetting->SetValueStep(1.0f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::Raw);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    Screen->Initialize(InOwningPlayer);

    return Screen;
}
