// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_Inventory.h"

#include "Components/TileView.h"

#include "HUD/InventorySlotData.h"
#include "Core/CH_Master.h"

#define CH Cast<ACH_Master>(GetOwningPlayerPawn())

void UUW_Inventory::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    this->OnInventoryUpdate();
    return;
}

void UUW_Inventory::OnInventoryUpdate()
{
    this->TileView->ClearListItems();

    for (int32 i = 0; i < CH->GetInventorySize(); ++i)
    {
        UInventorySlotData* InventorySlotData = NewObject<UInventorySlotData>(this);
        InventorySlotData->Accumulated = CH->GetInventorSlot(i);
        this->TileView->AddItem(InventorySlotData);
    
        continue;
    }

    return;
}

#undef CH
