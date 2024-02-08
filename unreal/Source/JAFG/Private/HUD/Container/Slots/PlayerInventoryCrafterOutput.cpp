// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Slots/PlayerInventoryCrafterOutput.h"

#include "Core/CH_Master.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CH Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_PlayerInventoryCrafterOutput::OnClicked()
{
    CH->OnInventoryCrafterDeliveryClicked(true);
    return;
}

#undef CH
