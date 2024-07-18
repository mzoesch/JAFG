// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuTabBar.h"

#include "Player/WorldPlayerController.h"
#include "TabBar/JAFGTabBarButton.h"

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

    this->VisibilityWillChangeDelegateHandle =
        WorldPlayerController->SubscribeToEscapeMenuVisibilityChangeRequest(
            FSlateVisibilityChnageRequestDelegateSignature::FDelegate::CreateUObject(this, &UEscapeMenuTabBar::OnVisibilityWillChange
        ));
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

bool UEscapeMenuTabBar::AllowClose(void) const
{
    return Super::AllowClose();
}

void UEscapeMenuTabBar::OnVisibilityWillChange(const bool bVisible, bool& bAllow, const TFunction<void(void)>& CallbackIfLateAllow)
{
    /*
     * Some other delegate bound to this multicast event hast already denied the close.
     * We do nothing and should for the next close broadcast request.
     */
    if (bAllow == false)
    {
        return;
    }

    if (bVisible)
    {
        return;
    }

    if (this->HasActiveTap() == false)
    {
        return;
    }

    if (this->AllowClose())
    {
        return;
    }

    bAllow = false;

    this->RequestToCloseCurrentTabAsync(CallbackIfLateAllow);

    return;
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

    check( this->ResumeEntryButtonWidgetClass && "Resume Entry Button Widget Class is not set." )
    FTabBarTabDescriptor Resume = UJAFGTabBar::GetDefaultTabDescriptor();
    Resume.Identifier        = "Resume";
    Resume.DisplayName       = "Resume";
    Resume.ButtonWidgetClass = this->ResumeEntryButtonWidgetClass;

    FTabBarTabDescriptor Achievements = UJAFGTabBar::GetDefaultTabDescriptor();
    Achievements.Identifier        = "Achievements";
    Achievements.DisplayName       = "Achievements";
    Achievements.Padding           = FMargin(0.0f, 200.0f, 0.0f, 0.0f);

    check( this->SettingsPanelWidgetClass && "Settings Panel Widget Class is not set." )
    FTabBarTabDescriptor Settings = UJAFGTabBar::GetDefaultTabDescriptor();
    Settings.Identifier        = "Settings";
    Settings.DisplayName       = "Settings";
    Settings.PanelWidgetClass  = this->SettingsPanelWidgetClass;

    FTabBarTabDescriptor SessionOptions = UJAFGTabBar::GetDefaultTabDescriptor();
    SessionOptions.Identifier        = "SessionOptions";
    SessionOptions.DisplayName       = "Session Options";

    check( this->ExitToMenuEntryButtonWidgetClass && "Exit To Menu Entry Button Widget Class is not set." )
    FTabBarTabDescriptor ExitToMenu = UJAFGTabBar::GetDefaultTabDescriptor();
    ExitToMenu.Identifier        = "ExitToMenu";
    ExitToMenu.DisplayName       = "Exit To Menu";
    ExitToMenu.Padding           = FMargin(0.0f, 50.0f, 0.0f, 0.0f);
    ExitToMenu.ButtonWidgetClass = this->ExitToMenuEntryButtonWidgetClass;

    check( this->ExitToDesktopEntryButtonWidgetClass && "Exit To Desktop Entry Button Widget Class is not set." )
    FTabBarTabDescriptor ExitToDesktop = UJAFGTabBar::GetDefaultTabDescriptor();
    ExitToDesktop.Identifier        = "ExitToDesktop";
    ExitToDesktop.DisplayName       = "Exit To Desktop";
    ExitToDesktop.ButtonWidgetClass = this->ExitToDesktopEntryButtonWidgetClass;

    this->RegisterConcreteTab(Resume);
    this->RegisterConcreteTab(Achievements);
    this->RegisterConcreteTab(Settings);
    this->RegisterConcreteTab(SessionOptions);
    this->RegisterConcreteTab(ExitToMenu);
    this->RegisterConcreteTab(ExitToDesktop);

    return;
}
