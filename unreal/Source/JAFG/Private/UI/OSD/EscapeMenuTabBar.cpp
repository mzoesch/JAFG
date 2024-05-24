// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuTabBar.h"

#include "Player/WorldPlayerController.h"
#include "TabBar/JAFGTabBarPanel.h"

UEscapeMenuTabBar::UEscapeMenuTabBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEscapeMenuTabBar::NativeConstruct(void)
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

    this->VisibilityShouldChangeDelegateHandle =
        WorldPlayerController->SubscribeToEscapeMenuVisibilityChanged(
            ADD_SLATE_VIS_DELG(UEscapeMenuTabBar::OnVisibilityShouldChange
        ));

    return;
}

void UEscapeMenuTabBar::NativeDestruct(void)
{
    Super::NativeDestruct();
}

void UEscapeMenuTabBar::OnVisibilityShouldChange(const bool bVisible)
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

void UEscapeMenuTabBar::RegisterAllTabs(void)
{
    Super::RegisterAllTabs();

    FTabBarTabDescriptor Resume = UJAFGTabBar::GetDefaultTabDescriptor();
    Resume.Identifier        = "Resume";
    Resume.DisplayName       = "Resume";

    FTabBarTabDescriptor Achievements = UJAFGTabBar::GetDefaultTabDescriptor();
    Achievements.Identifier        = "Achievements";
    Achievements.DisplayName       = "Achievements";
    Achievements.Padding           = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    FTabBarTabDescriptor Settings = UJAFGTabBar::GetDefaultTabDescriptor();
    Settings.Identifier        = "Settings";
    Settings.DisplayName       = "Settings";

    FTabBarTabDescriptor SessionOptions = UJAFGTabBar::GetDefaultTabDescriptor();
    SessionOptions.Identifier        = "SessionOptions";
    SessionOptions.DisplayName       = "Session Options";

    FTabBarTabDescriptor ExitToMenu = UJAFGTabBar::GetDefaultTabDescriptor();
    ExitToMenu.Identifier        = "ExitToMenu";
    ExitToMenu.DisplayName       = "Exit To Menu";

    FTabBarTabDescriptor ExitToDesktop = UJAFGTabBar::GetDefaultTabDescriptor();
    ExitToDesktop.Identifier        = "ExitToDesktop";
    ExitToDesktop.DisplayName       = "Exit To Desktop";

    this->RegisterConcreteTab(Resume);
    this->RegisterConcreteTab(Achievements);
    this->RegisterConcreteTab(Settings);
    this->RegisterConcreteTab(SessionOptions);
    this->RegisterConcreteTab(ExitToMenu);
    this->RegisterConcreteTab(ExitToDesktop);

    return;
}
