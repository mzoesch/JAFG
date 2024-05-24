// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/JAFGFrontEnd.h"

#include "TabBar/JAFGTabBarButton.h"

UJAFGFrontEnd::UJAFGFrontEnd(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGFrontEnd::RegisterAllTabs(void)
{
    check( this->HostSessionPanelWidgetClass && "Join Session Widget Class is not set." )
    FTabBarTabDescriptor Host = UJAFGTabBar::GetDefaultTabDescriptor();
    Host.Identifier        = "Host";
    Host.DisplayName       = "Host Session";
    Host.PanelWidgetClass  = this->HostSessionPanelWidgetClass;

    check( this->JoinSessionPanelWidgetClass && "Join Session Panel Widget Class is not set." )
    FTabBarTabDescriptor Join = UJAFGTabBar::GetDefaultTabDescriptor();
    Join.Identifier       = "Join";
    Join.DisplayName      = "Join Session";
    Join.PanelWidgetClass = this->JoinSessionPanelWidgetClass;

    check( this->SettingsPanelWidgetClass && "Settings Panel Widget Class is not set." )
    FTabBarTabDescriptor Settings = UJAFGTabBar::GetDefaultTabDescriptor();
    Settings.Identifier       = "Settings";
    Settings.DisplayName      = "Settings";
    Settings.PanelWidgetClass = this->SettingsPanelWidgetClass;
    Settings.Padding          = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    check( this->QuitGameButtonWidgetClass && "Quit Game Widget Class is not set." )
    FTabBarTabDescriptor Quit = UJAFGTabBar::GetDefaultTabDescriptor();
    Quit.Identifier        = "Quit";
    Quit.DisplayName       = "Quit Game";
    Quit.ButtonWidgetClass = this->QuitGameButtonWidgetClass;
    Quit.Padding           = FMargin(0.0f, 25.0f, 0.0f, 0.0f);

    this->RegisterConcreteTab(Host);
    this->RegisterConcreteTab(Join);
    this->RegisterConcreteTab(Settings);
    this->RegisterConcreteTab(Quit);

    return;
}
