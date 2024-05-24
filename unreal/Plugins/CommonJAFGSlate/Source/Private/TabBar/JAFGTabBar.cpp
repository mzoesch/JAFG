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

void UJAFGTabBar::UnfocusAllTabs(void)
{
    this->ActiveTabIdentifier = UJAFGTabBar::NoActiveTabIdentifier;
    this->WS_PanelContainer->SetActiveWidgetIndex(this->FindIndexOfIdentifier(this->ActiveTabIdentifier));
    this->OnTabPressedEvent.Broadcast(this->ActiveTabIdentifier);

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

void UJAFGTabBar::RegisterConcreteTab(const FTabBarTabDescriptor& TabDescriptor)
{
    if (TabDescriptor.ButtonWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Button widget class is invalid for %s.", *TabDescriptor.Identifier)
        return;
    }
    if (TabDescriptor.PanelWidgetClass == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Panel widget class is invalid for %s.", *TabDescriptor.Identifier)
        return;
    }

    UJAFGTabBarButton* Button = CreateWidget<UJAFGTabBarButton>(this, TabDescriptor.ButtonWidgetClass);
    FButtonEntryDescriptor ButtonData = FButtonEntryDescriptor();
    ButtonData.Owner      = this;
    ButtonData.Identifier = TabDescriptor.Identifier;
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

    this->WS_PanelContainer->AddChild(CreateWidget<UJAFGTabBarPanel>(this, TabDescriptor.PanelWidgetClass));

    this->RegisteredTabIdentifiersInOrder.Add(TabDescriptor.Identifier);

    return;
}

FTabBarTabDescriptor UJAFGTabBar::GetDefaultTabDescriptor(void)
{
    FTabBarTabDescriptor DefaultDescriptor = FTabBarTabDescriptor();

    const UCommonJAFGSlateDeveloperSettings* Settings = GetDefault<UCommonJAFGSlateDeveloperSettings>();

    DefaultDescriptor.ButtonWidgetClass = Settings->DefaultTabBarButtonWidgetClass;
    DefaultDescriptor.PanelWidgetClass  = Settings->DefaultTabBarPanelWidgetClass;

    return DefaultDescriptor;
}

void UJAFGTabBar::OnTabPressed(const FString& Identifier)
{
    if (UJAFGTabBarBase* Child = Cast<UJAFGTabBarBase>(this->WS_PanelContainer->GetActiveWidget()))
    {
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
