// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Container/Slots/CharacterInventorySlot.h"

#include "Network/NetworkStatics.h"
#include "UI/HUD/Container/Slots/SlateSlotData.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER \
    Cast<AWorldCharacter>(this->GetOwningPlayerPawn())
#define CHECKED_OWNING_CHARACTER \
    check( this->GetOwningPlayerPawn() ) \
    check( Cast<AWorldCharacter>(this->GetOwningPlayerPawn()) ) \
    Cast<AWorldCharacter>(this->GetOwningPlayerPawn())

void UCharacterInventorySlot::OnPrimaryClicked(void)
{
    bool bContentsChanged = false;

    CHECKED_OWNING_CHARACTER->Inventory[this->SlateSlotData->Index].OnPrimaryClicked(OWNING_CHARACTER, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    CHECKED_OWNING_CHARACTER->AskForInventoryChangeDelegateBroadcast();

    if ((UNetworkStatics::IsStandalone(this) || (UNetworkStatics::IsSafeListenServer(this) && OWNING_CHARACTER->IsLocallyControlled())) == false)
    {
        CHECKED_OWNING_CHARACTER->OnInventorySlotPrimaryClicked_ServerRPC(this->SlateSlotData->Index);
    }

    return;
}

void UCharacterInventorySlot::OnSecondaryClicked(void)
{
}

#undef OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
