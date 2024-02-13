// © 2023 mzoesch. All rights reserved.

#include "Lib/FAccumulated.h"

#include "World/WorldVoxel.h"

const FAccumulated FAccumulated::NullAccumulated = FAccumulated(EWorldVoxel::VoxelNull, 0);

FAccumulated::FAccumulated()
{
    this->Accumulated   = EWorldVoxel::VoxelNull;
    this->Amount        = 0;

    return;
}

FAccumulated::FAccumulated(const int InAccumulated)
{
    this->FAccumulated::FAccumulated(InAccumulated, 1);
    return;
}

FAccumulated::FAccumulated(const int InAccumulated, const int InAmount)
{
    this->Accumulated   = InAccumulated;
    this->Amount        = InAmount;

    return;
}

FString FAccumulated::ToString() const
{
    return FString::Printf(TEXT("FAccumulated {Voxel: %d,Amount: %d}"), this->Accumulated, this->Amount);
}
