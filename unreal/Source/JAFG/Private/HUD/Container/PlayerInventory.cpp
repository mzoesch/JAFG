// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/PlayerInventory.h"

#include "Components/TileView.h"

#include "Core/CH_Master.h"
#include "HUD/Container/Slots/PlayerInventoryCrafterOutput.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define MASTER_CHARACTER Cast<ACH_Master>(GetOwningPlayerPawn())

void UW_PlayerInventory::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    this->MarkAsDirty();
    return;
}

void UW_PlayerInventory::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    bool bDiscardTick = false;
    Super::NativeTickImpl(MyGeometry, InDeltaTime, bDiscardTick);
    if (bDiscardTick == true)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("UW_PlayerInventory::NativeTick: Container is dirty. Refreshing..."));
    this->RefreshCharacterInventory();
}

void UW_PlayerInventory::RefreshCharacterInventory()
{
    Super::RefreshCharacterInventory();

    this->CharacterInventoryCrafterSlots->ClearListItems();
    
    for (int8 i = 0; i < 4; ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);
    
        Data->Index        = i;

        if (MASTER_CHARACTER->GetInventoryCrafterSize() <= i)
        {
            UE_LOG(LogTemp, Warning, TEXT("Crafter out of bounds for index %d -> %d"), i, MASTER_CHARACTER->GetInventoryCrafterSize());
            Data->Accumulated  = FAccumulated::NullAccumulated;
        }
        else
        {
            Data->Accumulated  = MASTER_CHARACTER->GetInventoryCrafterSlot(i);
        }

        this->CharacterInventoryCrafterSlots->AddItem(Data);
        
        continue;
    }
    
    UContainerSlotData* InventoryCrafterOutputData          = NewObject<UContainerSlotData>(this);
    InventoryCrafterOutputData->Index                       = 0;
    InventoryCrafterOutputData->Accumulated                 = MASTER_CHARACTER->GetInventoryCrafterProduct();
    this->CharacterInventoryCrafterOutput->ContainerSlotData   = InventoryCrafterOutputData;
    this->CharacterInventoryCrafterOutput->RenderSlot();
    
}

#undef MASTER_CHARACTER
