// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollection.h"
#include "SettingsData/GameSettingDataSource.h"
#include "SettingsData/GameSettingValueScalar.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeGameplaySettings(void)
{
    UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
    Screen->SetIdentifier("Gameplay");
    Screen->SetDisplayName("Gameplay");

    {
        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("FirstTestCollection");
        Subsection->SetDisplayName("FirstSubsection");
        Screen->AddSetting(Subsection);

        {
            UGameSettingValueScalar* ConcreteSetting = NewObject<UGameSettingValueScalar>();
            ConcreteSetting->SetIdentifier("MyTestSetting");
            ConcreteSetting->SetDisplayName("My Test Setting");

            ConcreteSetting->SetValueGetter(MakeShared<FGameSettingDataSource>(TArray<FString>({
                ((void)sizeof(&UJAFGLocalPlayer::GetLocalSettings), L"GetLocalSettings"),
                ((void)sizeof(&UJAFGSettingsLocal::GetMasterVolume), L"GetMasterVolume")
            })));
            ConcreteSetting->SetValueSetter(MakeShared<FGameSettingDataSource>(TArray<FString>({
                ((void)sizeof(&UJAFGLocalPlayer::GetLocalSettings), L"GetLocalSettings"),
                ((void)sizeof(&UJAFGSettingsLocal::SetMasterVolume), L"SetMasterVolume")
            })));
            ConcreteSetting->SetDefaultValue(0.5f);
            ConcreteSetting->SetDisplayFormat(UGameSettingValueScalar::ZeroToOneAsPercent);

            Subsection->AddSetting(ConcreteSetting);
        }
    }

    // Network Quality
    // Headbobbing
    // camera movement feedback strength
    // autosave interfva
    // autosave notification
    // break notifications
    // mouse smoothing

    return NewObject<UGameSettingCollection>();
}
