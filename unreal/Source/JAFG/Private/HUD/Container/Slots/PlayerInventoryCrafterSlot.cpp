// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Slots/PlayerInventoryCrafterSlot.h"

#include "Core/CH_Master.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CH Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_PlayerInventoryCrafterSlot::OnClicked()
{
    CH->OnInventoryCrafterSlotClicked(this->ContainerSlotData->Index, true);
    return; 
}

#undef CH
