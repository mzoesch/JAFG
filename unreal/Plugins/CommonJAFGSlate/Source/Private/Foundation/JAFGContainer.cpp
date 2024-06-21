// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainer.h"

#include "Container.h"
#include "Components/TileView.h"
#include "Foundation/JAFGContainerSlot.h"

UJAFGContainer::UJAFGContainer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainer::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->BuildPlayerInventory();
    return;
}

void UJAFGContainer::BuildPlayerInventory(void)
{
    this->TV_PlayerInventory->ClearListItems();

    IContainer* OwningContainer = Cast<IContainer>(this->GetOwningPlayerPawn());
    if (OwningContainer == nullptr)
    {
        LOG_WARNING(LogCommonSlate, "Container is null")
        return;
    }

    for (int i = 0; i < OwningContainer->GetContainerSize(); ++i)
    {
        UJAFGContainerSlotData* Data = NewObject<UJAFGContainerSlotData>(this);
        Data->Index     = i;
        Data->Container = &OwningContainer->GetContainer();
        Data->Owner     = OwningContainer;

        this->TV_PlayerInventory->AddItem(Data);

        continue;
    }

    return;
}
