// © 2023 mzoesch. All rights reserved.

#include "Lib/FAccumulated.h"

#include "Core/GI_Master.h"
#include "World/WorldVoxel.h"

const FAccumulated FAccumulated::NullAccumulated = FAccumulated(EWorldVoxel::WV_Null, 0);

void FAccumulated::Init(const UGI_Master* InGIPtr)
{
    FAccumulated::ItemIndexStart = InGIPtr->GetItemIndexStart();
    return;
}

FAccumulated::FAccumulated()
{
    this->Accumulated   = EWorldVoxel::WV_Null;
    this->Amount        = 0;

    return;
}

FAccumulated::FAccumulated(const int AccumulatedIndex)
{
    this->FAccumulated::FAccumulated(AccumulatedIndex, FAccumulated::NullAccumulated.Accumulated == AccumulatedIndex ? 0 : 1);
    return;
}

FAccumulated::FAccumulated(const int AccumulatedIndex, const int InAmount)
{
    this->Accumulated   = AccumulatedIndex;
    this->Amount        = InAmount;

    return;
}

void FAccumulated::SafeAddAmount(const int InAmount, bool& bCouldProcess)
{
    if (InAmount == 0)
    {
        bCouldProcess = true;
        return;
    }

    int64   NewAmount  = this->Amount;
            NewAmount += InAmount;

    /* Underflow Error */
    if (NewAmount < 0)
    {
        bCouldProcess = false;
        return;
    }

    if (NewAmount == 0)
    {
        this->Accumulated   = NullAccumulated.Accumulated;
        this->Amount        = NullAccumulated.Amount;
        bCouldProcess       = true;
        return;
    }

    /* Overflow Error */
    if (NewAmount > 0xFFFF)
    {
        bCouldProcess = false;
        return;
    }

    this->Amount    += InAmount;
    bCouldProcess    = true;
            
    return;
}
