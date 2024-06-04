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
    LOG_WARNING(LogSystem, "Called. But not implemented.")
    return false;
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
