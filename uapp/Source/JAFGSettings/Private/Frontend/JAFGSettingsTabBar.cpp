// Copyright 2024 mzoesch. All rights reserved.

#include "Frontend/JAFGSettingsTabBar.h"
#include "JAFGGameSettingRegistry.h"

UJAFGSettingsTabBar::UJAFGSettingsTabBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGSettingsTabBar::NativeConstruct(void)
{
    this->OwningRegistry = this->GetOwningLocalPlayer()->GetSubsystem<UJAFGGameSettingRegistrySubsystem>()->LocalRegistry;

    Super::NativeConstruct();

    return;
}
