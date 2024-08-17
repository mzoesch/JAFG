// Copyright 2024 mzoesch. All rights reserved.

#include "Slot.h"
#include "Container.h"

bool FSlot::OnPrimaryClicked(IContainerOwner* Owner)
{
    if (Owner->CursorValue == Accumulated::Null && this->Content == Accumulated::Null)
    {
        return false;
    }

    if (Owner->CursorValue == Accumulated::Null)
    {
        Owner->CursorValue = this->Content;
        this->Content      = Accumulated::Null;
        return true;
    }

    if (this->Content == Accumulated::Null)
    {
        this->Content      = Owner->CursorValue;
        Owner->CursorValue = Accumulated::Null;
        return true;
    }

    if (this->Content == Owner->CursorValue)
    {
        bool bCouldProcess = false;
        this->Content.SafeAddAmount(Owner->CursorValue.Amount, bCouldProcess);
        if (bCouldProcess == false)
        {
            LOG_FATAL(LogSystem, "Failed with overflow.")
            return false;
        }
        Owner->CursorValue = Accumulated::Null;

        return true;
    }

    const FAccumulated Temp = this->Content;
    this->Content           = Owner->CursorValue;
    Owner->CursorValue      = Temp;

    return true;
}

bool FSlot::OnSecondaryClicked(IContainerOwner* Owner)
{
    if (Owner->CursorValue == Accumulated::Null && this->Content == Accumulated::Null)
    {
        return false;
    }

    if (Owner->CursorValue.IsNull())
    {
        Owner->CursorValue = FAccumulated(this->Content.AccumulatedIndex, this->Content.GetHalfAmount());
        this->Content.SubtractHalfAmount();
        return true;
    }

    if (this->Content.IsNull())
    {
        this->Content = FAccumulated(Owner->CursorValue.AccumulatedIndex);
        Owner->CursorValue.SafeAddAmount(-1);
        return true;
    }

    if (this->Content == Owner->CursorValue)
    {
        this->Content.SafeAddAmount(1);
        Owner->CursorValue.SafeAddAmount(-1);
        return true;
    }

    const FAccumulated Temp = this->Content;
    this->Content           = Owner->CursorValue;
    Owner->CursorValue      = Temp;

    return true;
}

bool FSlot::AddToFirstSuitableSlot(TArray<FSlot>& Container, const FAccumulated& Value)
{
    /* Check if there is an existing accumulated item already in the character's inventory. */
    for (FSlot& Slot : Container)
    {
        if (Slot.Content == Value)
        {
            bool bCouldProcess = false;
            Slot.Content.SafeAddAmount(Value.Amount, bCouldProcess);
            if (bCouldProcess == false)
            {
                LOG_FATAL(LogSystem, "Failed with overflow.")
                return false;
            }
            return true;
        }

        continue;
    }

    /* Add the new accumulated item to an empty inventory slot. */
    for (FSlot& Slot : Container)
    {
        if (Slot.Content == Accumulated::Null)
        {
            Slot.Content = Value;
            return true;
        }

        continue;
    }

    return false;
}
