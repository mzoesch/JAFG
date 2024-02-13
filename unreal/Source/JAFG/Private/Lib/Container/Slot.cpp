// © 2023 mzoesch. All rights reserved.

#include "Lib/Container/Slot.h"

#include "Core/CH_Master.h"

void FSlot::OnClicked(ACH_Master* Trigger, bool& bOutChangedData)
{
    bOutChangedData = true;
    
    if (Trigger->CursorHand == FAccumulated::NullAccumulated)
    {
        if (this->Content == FAccumulated::NullAccumulated)
        {
            bOutChangedData = false;
            return;
        }

        Trigger->CursorHand     = this->Content;
        this->Content           = FAccumulated::NullAccumulated;
        
        return;
    }

    /* Both the cursor hand and the slot's content have accumulated items. */
    if (this->Content != FAccumulated::NullAccumulated)
    {
        if (this->Content == Trigger->CursorHand)
        {
            this->Content.AddAmount(this->Content.GetAmount());
            Trigger->CursorHand = FAccumulated::NullAccumulated;

            return;
        }

        const FAccumulated Swap = this->Content;
        this->Content           = Trigger->CursorHand;
        Trigger->CursorHand     = Swap;

        return;
    }

    this->Content           = Trigger->CursorHand;
    Trigger->CursorHand     = FAccumulated::NullAccumulated;
    
    return;
}
