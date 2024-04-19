// Copyright 2024 mzoesch. All rights reserved.

#include "Jar/Slot.h"

#include "World/WorldCharacter.h"

void FSlot::OnPrimaryClicked(AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInRPC)
{
    if (bCalledInRPC == false && Owner->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Owner is not locally controlled.")
        return;
    }

    bOutChangedData = true;

    this->Content = Accumulated::Null;

    return;
}
