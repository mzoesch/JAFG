// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/SettingRegistry.h"

#include "Registry/GameSetting.h"

void USettingRegistry::OnInitialize(void)
{
    this->GamePlaySettings      = this->InitializeGamePlaySettings();
    this->AudioSettings         = this->InitializeAudioSettings();
    this->VideoSettings         = this->InitializeVideoSettings();
    this->ControlsSettings      = this->InitializeControlsSettings();
    this->KeybindingsSettings   = this->InitializeKeybindingsSettings();
    this->UserInterfaceSettings = this->InitializeUserInterfaceSettings();
    this->DebugSettings         = this->InitializeDebugSettings();

    check( this->GamePlaySettings )
    check( this->AudioSettings )
    check( this->VideoSettings )
    check( this->ControlsSettings )
    check( this->KeybindingsSettings )
    check( this->UserInterfaceSettings )
    check( this->DebugSettings )

    this->RegisterSetting(this->GamePlaySettings);
    this->RegisterSetting(this->AudioSettings);
    this->RegisterSetting(this->VideoSettings);
    this->RegisterSetting(this->ControlsSettings);
    this->RegisterSetting(this->KeybindingsSettings);
    this->RegisterSetting(this->UserInterfaceSettings);
    this->RegisterSetting(this->DebugSettings);

    return;
}

UGameSetting* USettingRegistry::FindSettingByIdentifier(const FString& Identifier)
{
    for (UGameSetting* Setting : this->RegisteredSettings)
    {
        if (Setting->GetIdentifier() == Identifier)
        {
            return Setting;
        }

        continue;
    }

    return nullptr;
}

void USettingRegistry::RegisterSetting(UGameSetting* InSetting)
{
    check( InSetting )

    this->TopLevelSettings.Add(InSetting);
    InSetting->SetRegistry(this);
    this->RegisterInnerSetting(InSetting);

    return;
}

void USettingRegistry::RegisterInnerSetting(UGameSetting* InSetting)
{
    check( InSetting )

#if !UE_BUILD_SHIPPING
    ensureAlwaysMsgf(!this->RegisteredSettings.Contains(InSetting), TEXT("This setting is already registered."));
    ensureAlwaysMsgf(
        nullptr == RegisteredSettings.FindByPredicate(
            [&InSetting] (const UGameSetting* ExistingSetting)
            {
                return InSetting->GetIdentifier() == ExistingSetting->GetIdentifier();
            }
        ), TEXT("A setting with this identifier has already been registered. They must be unique within a registry.")
    );
#endif /* !UE_BUILD_SHIPPING */

    this->RegisteredSettings.Add(InSetting);

    for (UGameSetting* ChildSetting : InSetting->GetChildSettings())
    {
        this->RegisterInnerSetting(ChildSetting);
    }

    return;
}
