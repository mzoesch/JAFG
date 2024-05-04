// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenu.h"

#include "Concretes/CommonBarEntryWidget.h"
#include "Player/WorldPlayerController.h"

UEscapeMenu::UEscapeMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEscapeMenu::NativeConstruct(void)
{
    Super::NativeConstruct();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Owning Player controller is not of type AWorldPlayerController but HUD decided to spawn this widget anyway.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Owning Player controller is invalid.")
#endif /* !WITH_EDITOR */
    }

    this->EscapeMenuVisibilityChangedDelegateHandle = WorldPlayerController->SubscribeToEscapeMenuVisibilityChanged(ADD_SLATE_VIS_DELG(UEscapeMenu::OnEscapeMenuVisibilityChanged));

    this->RegisterAllTabs();

    return;
}

void UEscapeMenu::NativeDestruct(void)
{
    Super::NativeDestruct();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());

    if (WorldPlayerController == nullptr)
    {
        return;
    }

    if (WorldPlayerController->UnSubscribeToEscapeMenuVisibilityChanged(this->EscapeMenuVisibilityChangedDelegateHandle) == false)
    {
        LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Escape Menu Visibility Changed event.")
    }

    return;
}

void UEscapeMenu::OnEscapeMenuVisibilityChanged(const bool bVisible)
{
    if (bVisible)
    {
        this->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}

void UEscapeMenu::RegisterAllTabs(void)
{
    check( this->DefaultEntryWidget && "Default Entry Widget is not set." )

    FCommonBarTabDescriptor Resume;
    Resume.Identifier       = TEXT("Resume");
    Resume.DisplayName      = TEXT("Resume");
    Resume.EntryWidgetClass = this->ResumeWidgetClass ? this->ResumeWidgetClass : this->DefaultEntryWidget;

    FCommonBarTabDescriptor Achievements;
    Achievements.Identifier       = TEXT("Achievements");
    Achievements.DisplayName      = TEXT("Achievements");
    Achievements.EntryWidgetClass = this->AchievementsWidgetClass ? this->AchievementsWidgetClass : this->DefaultEntryWidget;
    Achievements.Padding          = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    FCommonBarTabDescriptor Settings;
    Settings.Identifier       = TEXT("Settings");
    Settings.DisplayName      = TEXT("Settings");
    Settings.EntryWidgetClass = this->SettingsWidgetClass ? this->SettingsWidgetClass : this->DefaultEntryWidget;

    FCommonBarTabDescriptor SessionOptions;
    SessionOptions.Identifier       = TEXT("SessionOptions");
    SessionOptions.DisplayName      = TEXT("Session Options");
    SessionOptions.EntryWidgetClass = this->SessionOptionsWidgetClass ? this->SessionOptionsWidgetClass : this->DefaultEntryWidget;

    FCommonBarTabDescriptor ExitToMenu;
    ExitToMenu.Identifier       = TEXT("ExitToMenu");
    ExitToMenu.DisplayName      = TEXT("Exit to Menu");
    ExitToMenu.EntryWidgetClass = this->ExitToMainMenuWidgetClass ? this->ExitToMainMenuWidgetClass : this->DefaultEntryWidget;
    ExitToMenu.Padding          = FMargin(0.0f, 25.0f, 0.0f, 0.0f);

    FCommonBarTabDescriptor ExitToDesktop;
    ExitToDesktop.Identifier       = TEXT("ExitToDesktop");
    ExitToDesktop.DisplayName      = TEXT("Exit to Desktop");
    ExitToDesktop.EntryWidgetClass = this->ExitToDesktopWidgetClass ? this->ExitToDesktopWidgetClass : this->DefaultEntryWidget;

    this->RegisterTab(Resume);
    this->RegisterTab(Achievements);
    this->RegisterTab(Settings);
    this->RegisterTab(SessionOptions);
    this->RegisterTab(ExitToMenu);
    this->RegisterTab(ExitToDesktop);

    return;
}
