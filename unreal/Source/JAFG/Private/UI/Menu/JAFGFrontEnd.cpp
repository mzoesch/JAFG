// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/JAFGFrontEnd.h"

#include "Concretes/CommonBarEntryWidget.h"

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
    check(this->NewSessionWidgetClass && "New Session Widget Class is not set.");
    FCommonBarTabDescriptor New;
    New.Identifier       = "New";
    New.DisplayName      = "New Session";
    New.EntryWidgetClass = this->NewSessionWidgetClass;

    check(this->JoinSessionWidgetClass && "Join Session Widget Class is not set.");
    FCommonBarTabDescriptor Join;
    Join.Identifier       = "Join";
    Join.DisplayName      = "Join Session";
    Join.EntryWidgetClass = this->JoinSessionWidgetClass;

    check(this->SettingsWidgetClass && "Settings Widget Class is not set.");
    FCommonBarTabDescriptor Settings;
    Settings.Identifier       = "Settings";
    Settings.DisplayName      = "Settings";
    Settings.EntryWidgetClass = this->SettingsWidgetClass;
    Settings.Padding          = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    check(this->QuitGameWidgetClass && "Quit Game Widget Class is not set.");
    FCommonBarTabDescriptor Quit;
    Quit.Identifier       = "Quit";
    Quit.DisplayName      = "Quit Game";
    Quit.EntryWidgetClass = this->QuitGameWidgetClass;
    Quit.Padding          = FMargin(0.0f, 25.0f, 0.0f, 0.0f);

    this->RegisterTab(New);
    this->RegisterTab(Join);
    this->RegisterTab(Settings);
    this->RegisterTab(Quit);

    return;
}
