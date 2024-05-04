// Copyright 2024 mzoesch. All rights reserved.

#include "Concretes/CommonBarWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Concretes/CommonBarPanelWidget.h"
#include "Concretes/CommonBarEntryWidget.h"

UCommonBarWidget::UCommonBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UCommonBarWidget::UnfocusAllTabs(void)
{
    this->ActiveTabIdentifier = UCommonBarWidget::NoActiveTabIdentifier;
    this->OnTabPressedEvent.Broadcast(this->ActiveTabIdentifier);
    this->WS_BarPanels->SetActiveWidgetIndex(this->FindIndexOfIdentifier(this->ActiveTabIdentifier));

    return;
}

void UCommonBarWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->WS_BarPanels->ClearChildren();
    this->P_BarEntries->ClearChildren();

    /* The first entry must always be the fallback to an empty (no tab is selected screen). */
    this->WS_BarPanels->AddChild(this->WidgetTree->ConstructWidget<UOverlay>());
    this->RegisteredTabIdentifiersInOrder.Add(UCommonBarWidget::NoActiveTabIdentifier);

    return;
}

void UCommonBarWidget::NativeDestruct(void)
{
    Super::NativeDestruct();
}

void UCommonBarWidget::RegisterTab(const FCommonBarTabDescriptor& Descriptor)
{
    check( Descriptor.EntryWidgetClass )

    UCommonBarEntryWidget* Tab = CreateWidget<UCommonBarEntryWidget>(this, Descriptor.EntryWidgetClass);
    FPassedTabDescriptor PassedTabDescriptor;
    PassedTabDescriptor.Owner      = this;
    PassedTabDescriptor.Descriptor = Descriptor;
    Tab->PassDataToWidget(PassedTabDescriptor);
    this->P_BarEntries->AddChild(Tab);

    if (Descriptor.PanelWidgetClass != nullptr)
    {
        this->WS_BarPanels->AddChild(CreateWidget<UCommonBarPanelWidget>(this, Descriptor.PanelWidgetClass));
    }
    else
    {
        /*
         * An empty overlay added. This should generally not be visible and is just there so that we do not have
         * to deal with different indices in the WidgetSwitcher and the RegisteredTabIdentifiersInOrder.
         */
        this->WS_BarPanels->AddChild(this->WidgetTree->ConstructWidget<UOverlay>());
    }

    this->RegisteredTabIdentifiersInOrder.Add(Descriptor.Identifier);

    return;
}

void UCommonBarWidget::OnTabPressed(const FString& Identifier)
{
    if (this->ActiveTabIdentifier == Identifier)
    {
        this->ActiveTabIdentifier = UCommonBarWidget::NoActiveTabIdentifier;
    }
    else
    {
        this->ActiveTabIdentifier = Identifier;
    }

    this->OnTabPressedEvent.Broadcast(this->ActiveTabIdentifier);
    this->WS_BarPanels->SetActiveWidgetIndex(this->FindIndexOfIdentifier(this->ActiveTabIdentifier));

    return;
}

int32 UCommonBarWidget::FindIndexOfIdentifier(const FString& Identifier) const
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
