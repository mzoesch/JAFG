// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/SettingsTabBar.h"

#include "CustomSettingsLocalPlayer.h"
#include "Frontend/SettingsTabBarPanel.h"
#include "SettingsData/GameSettingCollections.h"
#include "SettingsData/GameSettingRegistry.h"

USettingsTabBar::USettingsTabBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void USettingsTabBar::NativeConstruct(void)
{
    Super::NativeConstruct();

    if (this->OwningRegistry == nullptr || !this->OwningRegistry->IsValidLowLevel())
    {
        LOG_FATAL(LogCommonSlate, "Local Settings Registry is invalid.")
        return;
    }

    return;
}

FTabBarTabDescriptor USettingsTabBar::GetDefaultSettingsTabDescriptor(void) const
{
    FTabBarTabDescriptor Descriptor = UJAFGTabBar::GetDefaultTabDescriptor();
    Descriptor.PanelWidgetClass = this->TabBarPanelClass;
    return Descriptor;
}

void USettingsTabBar::RegisterAllTabs(void)
{
    Super::RegisterAllTabs();

    for (const TObjectPtr<UGameSetting> Setting : this->OwningRegistry->GetTopLevelSettings())
    {
        if (Setting == nullptr || !Setting->IsValidLowLevel())
        {
#if WITH_EDITOR
            LOG_ERROR(LogGameSettings, "Invalid Setting received.")
#else /* WITH_EDITOR */
            LOG_FATAL(LogGameSettings, "Invalid Setting received.")
#endif /* !ITH_EDITOR */
            continue;
        }

        if (Setting->IsA(ULazyGameSettingCollection::StaticClass()))
        {
            Cast<ULazyGameSettingCollection>(Setting)->LazyInitialize(
                Cast<UCustomSettingsLocalPlayer>(GEngine->GetGamePlayer(this->GetWorld(), 0))
            );
        }

        FTabBarTabDescriptor TopLevel = this->GetDefaultSettingsTabDescriptor();
        TopLevel.Identifier    = Setting->GetIdentifier();
        TopLevel.DisplayName   = Setting->GetDisplayName().ToString();
        TopLevel.PanelPassData = new FSettingsPassData(Setting);

        this->RegisterConcreteTab(TopLevel);

        continue;
    }

    return;
}
