// © 2023 mzoesch. All rights reserved.


#include "HUD/UW_Inventory.h"

#include "Components/Image.h"
#include "Components/TileView.h"

#include "HUD/UW_HotbarSlot.h"
#include "Core/GI_Master.h"
#include "HUD/UW_InventorySlotV2.h"
#include "HUD/InventorySlotData.h"

void UUW_Inventory::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->TileView->ClearListItems();

    for (int32 i = 0; i < 5; ++i)
    {
        UInventorySlotData* InventorySlotData = NewObject<UInventorySlotData>(this);
        InventorySlotData->Amount = i;
        
        this->TileView->AddItem(InventorySlotData);
    
        continue;
    }

    UE_LOG(LogTemp, Warning, TEXT("UUW_Inventory::NativeOnInitialized: %d"), this->TileView->GetNumItems());

    return;
}

void UUW_Inventory::OnInventoryUpdate()
{
    UE_LOG(LogTemp, Warning, TEXT("UUW_Inventory::OnInventoryUpdate: %d"), this->TileView->GetNumItems());
}
