// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Settings/SettingsPanelWidget.h"

#include "Concretes/CommonBarEntryWidget.h"

USettingsPanelWidget::USettingsPanelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

USettingsBarPanelWidget::USettingsBarPanelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void USettingsBarPanelWidget::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->RegisterAllTabs();
    return;
}

void USettingsBarPanelWidget::RegisterAllTabs(void)
{
    check( this->DefaultEntryWidget && "Default Entry Widget is not set." )

    FCommonBarTabDescriptor Gameplay;
    Gameplay.Identifier       = "Gameplay";
    Gameplay.DisplayName      = "Gameplay";
    Gameplay.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor Audio;
    Audio.Identifier       = "Audio";
    Audio.DisplayName      = "Audio";
    Audio.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor Video;
    Video.Identifier       = "Video";
    Video.DisplayName      = "Video";
    Video.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor Controls;
    Controls.Identifier       = "Controls";
    Controls.DisplayName      = "Controls";
    Controls.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor Keybindings;
    Keybindings.Identifier       = "Keybindings";
    Keybindings.DisplayName      = "Keybindings";
    Keybindings.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor UserInterface;
    UserInterface.Identifier       = "UserInterface";
    UserInterface.DisplayName      = "User Interface";
    UserInterface.EntryWidgetClass = this->DefaultEntryWidget;

    FCommonBarTabDescriptor Debug;
    Debug.Identifier       = "Debug";
    Debug.DisplayName      = "Debug";
    Debug.EntryWidgetClass = this->DefaultEntryWidget;

    this->RegisterTab(Gameplay);
    this->RegisterTab(Audio);
    this->RegisterTab(Video);
    this->RegisterTab(Controls);
    this->RegisterTab(Keybindings);
    this->RegisterTab(UserInterface);
    this->RegisterTab(Debug);

    return;
}
