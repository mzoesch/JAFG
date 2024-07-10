// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainer.h"

#include "CommonJAFGSlateDeveloperSettings.h"
#include "Container.h"
#include "Components/JAFGTextBlock.h"
#include "Components/TileView.h"
#include "Foundation/JAFGContainerSlot.h"

UJAFGContainer::UJAFGContainer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainer::BuildDeferred(void)
{
    this->OnBuild();
}

void UJAFGContainer::NativeConstruct(void)
{
    Super::NativeConstruct();

    if (this->bDeferredBuild)
    {
        this->TryUpdateDisplayNames();
        return;
    }

    this->OnBuild();

    return;
}

void UJAFGContainer::OnBuild(void)
{
    this->TryUpdateDisplayNames();
    this->BuildPlayerInventory();

    return;
}

void UJAFGContainer::TryUpdateDisplayNames(void)
{
    if (this->TextBlock_PlayerInventoryDisplayName)
    {
        this->TextBlock_PlayerInventoryDisplayName->SetText(FText::FromString(this->GetPlayerInventoryDisplayName()));
    }

    return;
}

void UJAFGContainer::BuildPlayerInventory(void)
{
    this->BuildContainerWithCommonLogic(Cast<IContainer>(this->GetOwningPlayerPawn()), this->ScrollBox_PlayerInventoryWrapper, this->TV_PlayerInventory);
}

void UJAFGContainer::BuildContainerWithCommonLogic(IContainer* Container, UJAFGScrollBox* ScrollBox_TargetWrapper, UTileView* TileView_Target)
{
    TileView_Target->ClearListItems();

    if (Container == nullptr)
    {
        TileView_Target->SetVisibility(ESlateVisibility::Collapsed);

        ScrollBox_TargetWrapper->AddChild(CreateWidget<UJAFGUserWidget>(this, GetDefault<UCommonJAFGSlateDeveloperSettings>()->WaitingForContainerContentWidgetClass));

        return;
    }

    if (ScrollBox_TargetWrapper->GetChildrenCount() > 1)
    {
        const int32 Index = ScrollBox_TargetWrapper->GetChildIndex(TileView_Target);
        if (Index == INDEX_NONE)
        {
            LOG_FATAL(LogCommonSlate, "TileView not found in ScrollBox. Cannot proceed to build on: %s.", *TileView_Target->GetName())
            return;
        }

        UWidget* Widget = ScrollBox_TargetWrapper->GetChildAt(Index);
        if (Widget == nullptr)
        {
            LOG_FATAL(LogCommonSlate, "Widget is invalid. Cannot proceed to build on: %s.", *TileView_Target->GetName())
            return;
        }

        ScrollBox_TargetWrapper->ClearChildren();
        ScrollBox_TargetWrapper->AddChild(Widget);
    }

    TileView_Target->SetVisibility(ESlateVisibility::Visible);

    for (int i = 0; i < Container->GetContainerSize(); ++i)
    {
        UJAFGContainerSlotData* Data = NewObject<UJAFGContainerSlotData>(this);
        Data->Index     = i;
        Data->Container = &Container->GetContainer();
        Data->Owner     = Container;

        TileView_Target->AddItem(Data);

        continue;
    }

    return;
}
