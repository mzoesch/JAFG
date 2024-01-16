// © 2023 mzoesch. All rights reserved.


#include "HUD/UW_Inventory.h"

#include "Components/Image.h"
#include "Components/TileView.h"

#include "HUD/UW_HotbarSlot.h"
#include "Core/GI_Master.h"
#include "HUD/UW_InventorySlotV2.h"

void UUW_Inventory::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    check(this->UWInventorySlotClass)
    
    this->TileView->ClearListItems();

    for (int32 i = 0; i < 5; ++i)
    {
        UUW_InventorySlotV2* InventorySlot = CreateWidget<UUW_InventorySlotV2>(this->GetWorld(), this->UWInventorySlotClass);
        this->TileView->AddItem(InventorySlot);
    
        continue;
    }

    UE_LOG(LogTemp, Warning, TEXT("UUW_Inventory::NativeOnInitialized: %d"), this->TileView->GetNumItems());

    return;
}

void UUW_Inventory::OnInventoryUpdate()
{
    UE_LOG(LogTemp, Warning, TEXT("UUW_Inventory::OnInventoryUpdate: %d"), this->TileView->GetNumItems());
}
