// Copyright 2024 mzoesch. All rights reserved.

#include "Jar/Slot.h"

#include "World/WorldCharacter.h"
#include "World/Entity/EntityWorldSubsystem.h"

void FSlot::OnPrimaryClicked(const AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC /* = false */)
{
    if (bCalledInServerRPC == false && Owner->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Owner is not locally controlled.")
        return;
    }

    bOutChangedData = true;

    this->Content = Accumulated::Null;

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
