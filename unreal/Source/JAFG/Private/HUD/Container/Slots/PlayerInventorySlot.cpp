// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Slots/PlayerInventorySlot.h"

#include "Core/CH_Master.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CH Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_PlayerInventorySlot::OnClicked()
{
    CH->OnInventorySlotClicked(this->ContainerSlotData->Index, true);
    return;
}

#undef CH
