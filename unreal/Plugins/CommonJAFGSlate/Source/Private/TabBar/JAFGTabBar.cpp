// Copyright 2024 mzoesch. All rights reserved.

#include "TabBar/JAFGTabBar.h"

#include "CommonJAFGSlateDeveloperSettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"
#include "TabBar/JAFGTabBarButton.h"
#include "JAFGLogDefs.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "TabBar/JAFGTabBarPanel.h"

UJAFGTabBar::UJAFGTabBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGTabBar::UnfocusAllTabs(const TFunction<void(void)>& OnUnfocusedAllTabsDelegate)
{
    if (UJAFGTabBarBase* Child = Cast<UJAFGTabBarBase>(this->WS_PanelContainer->GetActiveWidget()))
    {
        if (Child->AllowClose() == false)
        {
            Child->TryToClose( [this, OnUnfocusedAllTabsDelegate] (void)
            {
                this->UnfocusAllTabs(OnUnfocusedAllTabsDelegate);
            });
            return;
        }

        Child->OnNativeMadeCollapsed();
        Child->OnMadeCollapsed();
    }

    this->ActiveTabIdentifier = UJAFGTabBar::NoActiveTabIdentifier;
    this->WS_PanelContainer->SetActiveWidgetIndex(this->FindIndexOfIdentifier(this->ActiveTabIdentifier));
    this->OnTabPressedEvent.Broadcast(this->ActiveTabIdentifier);

    OnUnfocusedAllTabsDelegate();

    return;
}

void UJAFGTabBar::SetVisibility(const ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    if (InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
    {
        this->OnNativeMadeCollapsed();
    }
    else
    {
        this->OnNativeMadeVisible();
    }

    return;
}

void UJAFGTabBar::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->P_EntryButtonContainer->ClearChildren();
    this->WS_PanelContainer->ClearChildren();
    this->RegisteredTabIdentifiersInOrder.Empty();

    /* The first entry must always be the fallback to an empty (no tab is selected) screen. */
    this->WS_PanelContainer->AddChild(this->WidgetTree->ConstructWidget<UOverlay>());
    this->WS_PanelContainer->SetActiveWidgetIndex(0);
    this->RegisteredTabIdentifiersInOrder.Add(UJAFGTabBar::NoActiveTabIdentifier);

    this->RegisterAllTabs();

    return;
}

void UJAFGTabBar::NativeDestruct(void)
{
    Super::NativeDestruct();
}

void UJAFGTabBar::OnNativeMadeVisible(void)
{
    Super::OnNativeMadeVisible();
}

void UJAFGTabBar::OnNativeMadeCollapsed(void)
{
    Super::OnNativeMadeCollapsed();

    if (this->HasActiveTap())
    {
        this->OnTabPressed(this->ActiveTabIdentifier);
    }

#if !UE_BUILD_SHIPPING
    if (this->HasActiveTap())
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "Tab bar has an active tab when it tryed to close just now.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "Tab bar has an active tab when it tryed to close just now.")
#endif /* !WITH_EDITOR */
    }
#endif

    return;
}

bool UJAFGTabBar::AllowClose(void) const
{
    if (Super::AllowClose() == false)
    {
        return false;
    }

    if (this->HasActiveTap() == false)
    {
        return true;
    }

    for (const UWidget* const Child : this->WS_PanelContainer->GetAllChildren())
    {
        if (const UJAFGTabBarBase* const Tab = Cast<UJAFGTabBarBase>(Child); Tab)
        {
            if (Tab->AllowClose() == false)
            {
                return false;
            }
        }
    }

    return true;
}

void UJAFGTabBar::TryToClose(const TFunction<void()>& CallbackIfLateAllow)
{
    Super::TryToClose(CallbackIfLateAllow);

    for (UWidget* const Child : this->WS_PanelContainer->GetAllChildren())
    {
        if (UJAFGTabBarBase* const Tab = Cast<UJAFGTabBarBase>(Child); Tab)
        {
            if (Tab->AllowClose() == false)
            {
                Tab->TryToClose(CallbackIfLateAllow);
                return;
            }
        }
    }

    LOG_ERROR(LogCommonSlate, "No tab found that disallows the close. Calling late delegate on next tick.")
    AsyncTask(ENamedThreads::GameThread, CallbackIfLateAllow);

    return;
}

void UJAFGTabBar::RegisterConcreteTab(const FTabBarTabDescriptor& TabDescriptor)
{
    if (TabDescriptor.ButtonWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Button widget class is invalid for %s.", *TabDescriptor.Identifier)
        return;
    }

    UJAFGTabBarButton* Button = CreateWidget<UJAFGTabBarButton>(this, TabDescriptor.ButtonWidgetClass);
    FButtonEntryDescriptor ButtonData = FButtonEntryDescriptor();
    ButtonData.Owner       = this;
    ButtonData.Identifier  = TabDescriptor.Identifier;
    ButtonData.DisplayName = TabDescriptor.DisplayName;
    Button->PassDataToWidget(ButtonData);
    UPanelSlot* MySlot = this->P_EntryButtonContainer->AddChild(Button);

    /* Kinda sketchy. Not a fan of this. But it works for now. */
    if (TabDescriptor.Padding != FMargin(0.0f))
    {
        if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(MySlot); VSlot)
        {
            VSlot->SetPadding(TabDescriptor.Padding);
        }
        else if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(MySlot); HSlot)
        {
            HSlot->SetPadding(TabDescriptor.Padding);
        }
        else
        {
            LOG_ERROR(LogCommonSlate, "The padding slot of %s is not supported. Not applying any padding.", *TabDescriptor.Identifier)
        }
    }

    if (TabDescriptor.PanelWidgetClass == nullptr)
    {
        this->WS_PanelContainer->AddChild(this->WidgetTree->ConstructWidget<UOverlay>());
    }
    else
    {
        UJAFGTabBarBase* Panel = CreateWidget<UJAFGTabBarBase>(this, TabDescriptor.PanelWidgetClass);
        if (TabDescriptor.PanelPassData != nullptr)
        {
            Panel->PassDataToWidget(*TabDescriptor.PanelPassData);
        }
        this->WS_PanelContainer->AddChild(Panel);
    }

    this->RegisteredTabIdentifiersInOrder.Add(TabDescriptor.Identifier);

    return;
}

FTabBarTabDescriptor UJAFGTabBar::GetDefaultTabDescriptorWithPanel(void)
{
    FTabBarTabDescriptor DefaultDescriptor = FTabBarTabDescriptor();

    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();

    DefaultDescriptor.ButtonWidgetClass = Settings->DefaultTabBarButtonWidgetClass;
    DefaultDescriptor.PanelWidgetClass  = Settings->DefaultTabBarPanelWidgetClass;

    return DefaultDescriptor;
}

FTabBarTabDescriptor UJAFGTabBar::GetDefaultTabDescriptor(void)
{
    FTabBarTabDescriptor DefaultDescriptor = FTabBarTabDescriptor();

    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();

    DefaultDescriptor.ButtonWidgetClass = Settings->DefaultTabBarButtonWidgetClass;
    DefaultDescriptor.PanelWidgetClass  = nullptr;

    return DefaultDescriptor;
}

void UJAFGTabBar::RequestToCloseCurrentTabAsync(const TFunction<void(void)>& CallbackIfLateAllow)
{
    if (this->HasActiveTap() == false)
    {
        LOG_WARNING(LogCommonSlate, "No active tab to close. Calling late delegate on next tick.")
        AsyncTask(ENamedThreads::GameThread, CallbackIfLateAllow);
        return;
    }

    if (this->AllowClose())
    {
        LOG_WARNING(LogCommonSlate, "Tab bar can be closed via normal close procedure. Calling late delegate on next tick.")
        AsyncTask(ENamedThreads::GameThread, CallbackIfLateAllow);
        return;
    }

    for (UWidget* const Child : this->WS_PanelContainer->GetAllChildren())
    {
        if (UJAFGTabBarBase* const Tab = Cast<UJAFGTabBarBase>(Child); Tab)
        {
            if (Tab->AllowClose() == false)
            {
                Tab->TryToClose(CallbackIfLateAllow);
                return;
            }
        }
    }

    LOG_ERROR(LogCommonSlate, "No tab found that disallows the close. Calling late delegate on next tick.")
    AsyncTask(ENamedThreads::GameThread, CallbackIfLateAllow);

    return;
}

void UJAFGTabBar::OnTabPressed(const FString& Identifier)
{
    if (UJAFGTabBarBase* Child = Cast<UJAFGTabBarBase>(this->WS_PanelContainer->GetActiveWidget()))
    {
        if (Child->AllowClose() == false)
        {
            Child->TryToClose( [this, Identifier] (void)
            {
                this->OnTabPressed(Identifier);
            });

            return;
        }

        Child->OnNativeMadeCollapsed();
        Child->OnMadeCollapsed();
    }

    if (this->ActiveTabIdentifier == Identifier)
    {
        this->ActiveTabIdentifier = UJAFGTabBar::NoActiveTabIdentifier;
    }
    else
    {
        this->ActiveTabIdentifier = Identifier;
    }

    this->WS_PanelContainer->SetActiveWidgetIndex(this->FindIndexOfIdentifier(this->ActiveTabIdentifier));
    this->OnTabPressedEvent.Broadcast(this->ActiveTabIdentifier);

    if (UJAFGTabBarBase* Child = Cast<UJAFGTabBarBase>(this->WS_PanelContainer->GetActiveWidget()))
    {
        Child->OnNativeMadeVisible();
        Child->OnMadeVisible();
    }

    return;
}

int32 UJAFGTabBar::FindIndexOfIdentifier(const FString& Identifier) const
{
    for (int32 Index = 0; Index < this->RegisteredTabIdentifiersInOrder.Num(); ++Index)
    {
        if (this->RegisteredTabIdentifiersInOrder[Index] == Identifier)
        {
            return Index;
        }

        continue;
    }

    return -1;
}
