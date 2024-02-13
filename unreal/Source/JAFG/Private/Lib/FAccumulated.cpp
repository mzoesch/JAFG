// © 2023 mzoesch. All rights reserved.

#include "Lib/FAccumulated.h"

#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"

const FAccumulated FAccumulated::NullAccumulated = FAccumulated(EWorldVoxel::WV_Null, 0);

FAccumulated::FAccumulated()
{
    this->Accumulated   = EWorldVoxel::WV_Null;
    this->Amount        = 0;

    return;
}

FAccumulated::FAccumulated(const int AccumulatedIndex)
{
    this->FAccumulated::FAccumulated(AccumulatedIndex, 1);
    return;
}

FAccumulated::FAccumulated(const int AccumulatedIndex, const int InAmount)
{
    this->Accumulated   = AccumulatedIndex;
    this->Amount        = InAmount;

    return;
}
