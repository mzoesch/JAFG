// Copyright 2024 mzoesch. All rights reserved.

#include "Jar/Slot.h"

#include "World/WorldCharacter.h"
#include "World/Entity/EntityWorldSubsystem.h"

void FSlot::OnPrimaryClicked(AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC /* = false */)
{
    if (bCalledInServerRPC == false && Owner->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Owner is not locally controlled.")
        return;
    }

    bOutChangedData = true;

    if (Owner->CursorHand == Accumulated::Null)
    {
        if (this->Content == Accumulated::Null)
        {
            bOutChangedData = false;
            return;
        }

        Owner->CursorHand = this->Content;
        this->Content     = Accumulated::Null;

        return;
    }

    if (this->Content != Accumulated::Null)
    {
        if (this->Content == Owner->CursorHand)
        {
            bool bCouldProcess = false;
            this->Content.SafeAddAmount(Owner->CursorHand.Amount, bCouldProcess);
            if (bCouldProcess == false)
            {
                LOG_FATAL(LogWorldChar, "Failed with an overflow error.")
                return;
            }

            Owner->CursorHand = Accumulated::Null;

            return;
        }

        const FAccumulated Swap = this->Content;
        this->Content           = Owner->CursorHand;
        Owner->CursorHand       = Swap;

        return;
    }

    this->Content     = Owner->CursorHand;
    Owner->CursorHand = Accumulated::Null;

    return;
}

void FSlot::OnSecondaryClicked(AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC /* = false */)
{
    if (bCalledInServerRPC == false && Owner->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Owner is not locally controlled.")
        return;
    }

    bOutChangedData = true;

    if (Owner->CursorHand == Accumulated::Null)
    {
        if (this->Content == Accumulated::Null)
        {
            bOutChangedData = false;
            return;
        }

        if (this->Content.Amount == 1)
        {
            Owner->CursorHand = this->Content;
            this->Content     = Accumulated::Null;
            return;
        }

        const int Half       = this->Content.Amount / 2;
        Owner->CursorHand    = FAccumulated(this->Content.AccumulatedIndex, this->Content.Amount - Half);
        this->Content.Amount = Half;

        return;
    }

    if (this->Content != Accumulated::Null)
    {
        if (this->Content == Owner->CursorHand)
        {
            bool bCouldProcess = false;
            this->Content.SafeAddAmount(1, bCouldProcess);
            if (bCouldProcess == false)
            {
                LOG_FATAL(LogWorldChar, "Failed with an overflow error.")
                return;
            }

            Owner->CursorHand.SafeAddAmount(-1, bCouldProcess);
            if (bCouldProcess == false)
            {
                LOG_FATAL(LogWorldChar, "Failed with an underflow error.")
                return;
            }

            return;
        }

        const FAccumulated Swap = this->Content;
        this->Content           = Owner->CursorHand;
        Owner->CursorHand       = Swap;

        return;
    }

    bool bCouldProcess = false;
    this->Content      = FAccumulated(Owner->CursorHand.AccumulatedIndex);
    Owner->CursorHand.SafeAddAmount(-1, bCouldProcess);
    if (bCouldProcess == false)
    {
        LOG_FATAL(LogWorldChar, "Failed with an underflow error.")
        return;
    }

    return;
}

void FSlot::OnDrop(const AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC /* = false */)
{
    bOutChangedData = false;

    if (this->Content == Accumulated::Null)
    {
        return;
    }

    const int32 DropAccumulatedIndex = this->Content.AccumulatedIndex;

    bool bCouldProcess = false;
    this->Content.SafeAddAmount(-1, bCouldProcess);
    if (bCouldProcess == false)
    {
        LOG_FATAL(LogWorldChar, "Failed with an underflow error.")
        return;
    }

    bOutChangedData = true;

    if (bCalledInServerRPC == false)
    {
        /*
         * Only the server can create drops.
         */
        return;
    }

    check( Owner )
    check( Owner->GetWorld() )
    check( Owner->GetWorld()->GetSubsystem<UEntityWorldSubsystem>() )
    Owner->GetWorld()->GetSubsystem<UEntityWorldSubsystem>()->CreateDrop(
        FAccumulated(DropAccumulatedIndex),
        Owner->IsLocallyControlled() ? Owner->GetTorsoTransform() : Owner->GetTorsoTransform_DedServer()
    );

    return;
}
