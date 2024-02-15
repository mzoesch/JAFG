// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Slots/PlayerInventoryCrafterSlot.h"

#include "Core/CH_Master.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CHARACTER Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_PlayerInventoryCrafterSlot::OnClicked()
{
    CHARACTER->OnInventoryCrafterSlotClicked(this->ContainerSlotData->Index, true);
    return; 
}

void UW_PlayerInventoryCrafterSlot::OnSecondaryClicked()
{
    CHARACTER->OnInventoryCrafterSlotSecondaryClicked(this->ContainerSlotData->Index, true);
    return;   
}

#undef CHARACTER
