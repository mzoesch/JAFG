// © 2023 mzoesch. All rights reserved.

#include "Lib/Container/Slot.h"

#include "Core/CH_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

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
            bool bCouldProcess = false;
            this->Content.SafeAddAmount(Trigger->CursorHand.Amount, bCouldProcess);
            if (bCouldProcess == false)
            {
                UIL_LOG(Fatal, TEXT("FSlot::OnClicked: this->Content.SafeAddAmount failed with an overflow error."));
                return;
            }
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

void FSlot::OnSecondaryClicked(ACH_Master* Trigger, bool& bOutChangedData)
{
    bOutChangedData = true;

    if (Trigger->CursorHand == FAccumulated::NullAccumulated)
    {
        if (this->Content == FAccumulated::NullAccumulated)
        {
            bOutChangedData = false;
            return;
        }

        if (this->Content.Amount == 1)
        {
            Trigger->CursorHand = this->Content;
            this->Content       = FAccumulated::NullAccumulated;
            return;
        }
        
        const int Half = this->Content.Amount / 2;
        Trigger->CursorHand = FAccumulated(this->Content.Accumulated, this->Content.Amount - Half);
        this->Content.Amount = Half;

        return;
    }

    /* Both the cursor hand and the slot's content have accumulated items. */
    if (this->Content != FAccumulated::NullAccumulated)
    {
        if (this->Content == Trigger->CursorHand)
        {
            bool bCouldProcess = false;

            this->Content.SafeAddAmount(1, bCouldProcess);
            if (bCouldProcess == false)
            {
                UIL_LOG(Fatal, TEXT("FSlot::OnSecondaryClicked: this->Content.SafeAddAmount failed with an overflow error."));
                return;
            }

            Trigger->CursorHand.SafeAddAmount(-1, bCouldProcess);
            if (bCouldProcess == false)
            {
                UIL_LOG(Fatal, TEXT("FSlot::OnSecondaryClicked: CursorHand.SafeAddAmount failed with an underflow error."));
                return;
            }

            return;
        }

        const FAccumulated Swap = this->Content;
        this->Content           = Trigger->CursorHand;
        Trigger->CursorHand     = Swap;
        
        return;
    }

    bool bCouldProcess  = false;
    this->Content       = FAccumulated(Trigger->CursorHand.Accumulated, 1);
    Trigger->CursorHand.SafeAddAmount(-1, bCouldProcess);
    if (bCouldProcess == false)
    {
        UIL_LOG(Fatal, TEXT("FSlot::OnSecondaryClicked: CursorHand.SafeAddAmount failed with an underflow error."));
        return;
    }
    
    return;
}

#undef UIL_LOG
