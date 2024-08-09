// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingValueKeyIn.h"
#include "SettingsData/JAFGInputSubsystem.h"

UGameSettingCollection* UJAFGGameSettingRegistry::InitializeKeybindingSettings(UJAFGLocalPlayer* InOwningPlayer)
{
    ULazyGameSettingCollection* LazyScreen = NewObject<ULazyGameSettingCollection>();
    LazyScreen->SetIdentifier("Keybindings");
    LazyScreen->SetDisplayName("Keybindings");

    UGameSettingCollection* Screen = LazyScreen;
    LazyScreen->SetLazyInitFunction( [Screen] (const UCustomSettingsLocalPlayer* InLazyOwningPlayer)
    {
        check( Screen )

        UGameSettingCollection* Subsection = NewObject<UGameSettingCollection>();
        Subsection->SetIdentifier("KeybindingsCollection");
        Subsection->SetDisplayName("Keybindings");
        Screen->AddSetting(Subsection);

        {
            // ReSharper disable once CppUseStructuredBinding
            for (const FLoadedInputAction& Mapping : InLazyOwningPlayer->GetLocalSettings()->GetAllLoadedInputActions())
            {
                UGameSettingValueKeyIn* ConcreteSetting = NewObject<UGameSettingValueKeyIn>();
                ConcreteSetting->SetIdentifier(Mapping.Name);
                ConcreteSetting->SetDisplayName(Mapping.Name);

                ConcreteSetting->SetInputData(InLazyOwningPlayer->GetSubsystem<UJAFGInputSubsystem>(), Mapping.Name);

                Subsection->AddSetting(ConcreteSetting);
            }
        }

        return;
    });

    return LazyScreen;
}
