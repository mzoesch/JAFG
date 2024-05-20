// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/JAFGFrontEnd.h"

#include "Concretes/CommonBarEntryWidget.h"
#include "Concretes/CommonBarPanelWidget.h"

UJAFGFrontEnd::UJAFGFrontEnd(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGFrontEnd::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->RegisterAllTabs();
    return;
}

void UJAFGFrontEnd::NativeDestruct(void)
{
    Super::NativeDestruct();
}

void UJAFGFrontEnd::RegisterAllTabs(void)
{
    check( this->HostSessionEntryWidgetClass && "Host Session Widget Class is not set. ")
    check( this->HostSessionPanelWidgetClass && "Join Session Widget Class is not set." )
    FCommonBarTabDescriptor Host;
    Host.Identifier       = "Host";
    Host.DisplayName      = "Host Session";
    Host.PanelWidgetClass = this->HostSessionPanelWidgetClass;
    Host.EntryWidgetClass = this->HostSessionEntryWidgetClass;

    check( this->JoinSessionEntryWidgetClass && "Join Session Widget Class is not set." )
    check( this->JoinSessionPanelWidgetClass && "Join Session Panel Widget Class is not set." )
    FCommonBarTabDescriptor Join;
    Join.Identifier       = "Join";
    Join.DisplayName      = "Join Session";
    Join.PanelWidgetClass = this->JoinSessionPanelWidgetClass;
    Join.EntryWidgetClass = this->JoinSessionEntryWidgetClass;

    check( this->SettingsEntryWidgetClass && "Settings Widget Class is not set." )
    check( this->SettingsPanelWidgetClass && "Settings Panel Widget Class is not set." )
    FCommonBarTabDescriptor Settings;
    Settings.Identifier       = "Settings";
    Settings.DisplayName      = "Settings";
    Settings.EntryWidgetClass = this->SettingsEntryWidgetClass;
    Settings.PanelWidgetClass = this->SettingsPanelWidgetClass;
    Settings.Padding          = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    check( this->QuitGameEntryWidgetClass && "Quit Game Widget Class is not set." )
    FCommonBarTabDescriptor Quit;
    Quit.Identifier       = "Quit";
    Quit.DisplayName      = "Quit Game";
    Quit.EntryWidgetClass = this->QuitGameEntryWidgetClass;
    Quit.Padding          = FMargin(0.0f, 25.0f, 0.0f, 0.0f);

    this->RegisterTab(Host);
    this->RegisterTab(Join);
    this->RegisterTab(Settings);
    this->RegisterTab(Quit);

    return;
}
