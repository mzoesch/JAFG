// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Encyclopedia/EncyclopediaTabBar.h"
#include "TabBar/JAFGTabBarDevPanel.h"

UEncyclopediaTabBar::UEncyclopediaTabBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEncyclopediaTabBar::RegisterAllTabs(void)
{
    Super::RegisterAllTabs();

    jcheck( this->AccumulatedPanelWidgetClass && "Accumulated Panel Widget Class is not set." )
    FTabBarTabDescriptor Accumulated = UJAFGTabBar::GetDefaultTabDescriptor();
    Accumulated.Identifier       = "Accumulated";
    Accumulated.DisplayName      = "Accumulated";
    Accumulated.PanelWidgetClass = this->AccumulatedPanelWidgetClass;

    this->RegisterConcreteTab(Accumulated);

    return;
}
