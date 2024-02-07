// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/PlayerInventory.h"

#include "Components/TileView.h"

#include "Lib/HUD/Container/ContainerSlotData.h"
#include "Core/CH_Master.h"

#define CH Cast<ACH_Master>(GetOwningPlayerPawn())

void UW_PlayerInventory::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    this->OnInventoryUpdate();
    return;
}

void UW_PlayerInventory::OnInventoryUpdate()
{
    this->PlayerInventorySlots->ClearListItems();
    this->PlayerInventoryCrafterSlots->ClearListItems();

    for (int32 i = 0; i < CH->GetInventorySize(); ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);

        Data->Index        = i;
        Data->Accumulated  = CH->GetInventoryAtSlot(i);

        this->PlayerInventorySlots->AddItem(Data);
    
        continue;
    }

    for (int8 i = 0; i < 4; ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);

        this->PlayerInventoryCrafterSlots->AddItem(Data);
        
        continue;
    }

    return;
}

#undef CH
