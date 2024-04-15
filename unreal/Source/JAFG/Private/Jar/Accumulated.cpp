// Copyright 2024 mzoesch. All rights reserved.

#include "Jar/Accumulated.h"

FAccumulated::FAccumulated(void)
{
    this->AccumulatedIndex = Accumulated::Null.AccumulatedIndex;
    this->Amount           = Accumulated::Null.Amount;

    return;
}

FAccumulated::FAccumulated(const int32 InAccumulatedIndex)
{
    this->AccumulatedIndex = InAccumulatedIndex;
    this->Amount           = Accumulated::Null.AccumulatedIndex == InAccumulatedIndex ? 0 : 1;

    return;
}

FAccumulated::FAccumulated(const int32 InAccumulatedIndex, const uint16 InAmount)
{
    this->AccumulatedIndex = InAccumulatedIndex;
    this->Amount           = InAmount;

    return;
}

void FAccumulated::SafeAddAmount(const int32 InAmount, bool& bCouldProcess)
{
    if (InAmount == 0)
    {
        bCouldProcess = true;
        return;
    }

    int64 NewAmount  = this->Amount;
          NewAmount += InAmount;

    /* Underflow Error */
    if (NewAmount < 0)
    {
        bCouldProcess = false;
        return;
    }

    if (NewAmount == 0)
    {
        this->AccumulatedIndex = Accumulated::Null.AccumulatedIndex;
        this->Amount           = Accumulated::Null.Amount;
        bCouldProcess          = true;

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
