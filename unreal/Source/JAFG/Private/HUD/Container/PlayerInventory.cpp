// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/PlayerInventory.h"

#include "Components/TileView.h"

#include "Core/CH_Master.h"
#include "HUD/Container/Slots/PlayerInventoryCrafterOutput.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CHARACTER Cast<ACH_Master>(GetOwningPlayerPawn())

void UW_PlayerInventory::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    bool bDiscardTick = false;
    Super::NativeTickImpl(MyGeometry, InDeltaTime, bDiscardTick);
    if (bDiscardTick == true)
    {
        return;
    }

    this->RefreshCharacterInventory();
}

void UW_PlayerInventory::OnDestroy(void)
{
    for (int i = 0; i < 4; ++i)
    {
        CHARACTER->GetInventoryCrafterSlotPtr(i)->OnDestroy(CHARACTER);
        continue;
    }

    return;
}

void UW_PlayerInventory::RefreshCharacterInventory(void)
{
    Super::RefreshCharacterInventory();

    this->CharacterInventoryCrafterSlots->ClearListItems();
    
    for (int8 i = 0; i < 4; ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);
    
        Data->Index        = i;
        Data->Accumulated  = CHARACTER->GetInventoryCrafterSlot(i);

        this->CharacterInventoryCrafterSlots->AddItem(Data);
        
        continue;
    }
    
    UContainerSlotData* InventoryCrafterOutputData           = NewObject<UContainerSlotData>(this);
    InventoryCrafterOutputData->Index                        = 0;
    InventoryCrafterOutputData->Accumulated                  = CHARACTER->GetInventoryCrafterProduct();
    this->CharacterInventoryCrafterOutput->ContainerSlotData = InventoryCrafterOutputData;
    this->CharacterInventoryCrafterOutput->RenderSlot();

    return;
}

#undef CHARACTER
