// Copyright 2024 mzoesch. All rights reserved.

#include "Settings/JAFGSettingScreen.h"

#include "Components/VerticalBox.h"
#include "Registry/RegistrySubsystem.h"
#include "Registry/SettingRegistry.h"
#include "Settings/SettingScreenSideTab.h"

void UJAFGSettingScreen::RegisterTab(const FSettingTabDescriptor& Descriptor)
{
    USettingScreenSideTab* Tab = CreateWidget<USettingScreenSideTab>(this->GetWorld(), this->SideTabClass);
    FPassedSettingTabDescriptor PassedDescriptor; PassedDescriptor.Descriptor = Descriptor;
    Tab->PassDataToWidget(PassedDescriptor);
    this->VB_SideTabs->AddChild(Tab);

    return;
}

UGameSettingCollection* UJAFGSettingScreen::GetSettingCollection(const FString& Identifier) const
{
    return this->GetRegistry()->FindSafeSettingByIdentifier<UGameSettingCollection>(Identifier);
}

USettingRegistry* UJAFGSettingScreen::GetRegistry(void) const
{
    if (const URegistrySubsystem* Subsystem = this->GetGameInstance()->GetSubsystem<URegistrySubsystem>())
    {
        return Subsystem->SettingRegistry;
    }

    LOG_FATAL(LogFrontEnd, "No registry subsystem found.")

    return nullptr;
}
