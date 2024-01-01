// © 2023 mzoesch. All rights reserved.

#include "Lib/FAccumulated.h"

#include "World/WorldVoxel.h"

const FAccumulated FAccumulated::NullAccumulated = FAccumulated(EWorldVoxel::VoxelNull, EItem::NullItem, 0);

FAccumulated::FAccumulated()
{
    this->Item = EItem::NullItem;
    this->Voxel = EWorldVoxel::VoxelNull;
    this->Amount = 0;

    return;
}

FAccumulated::FAccumulated(const int V)
{
    this->FAccumulated::FAccumulated(V, 1);
    return;
}

FAccumulated::FAccumulated(const EItem I)
{
    this->FAccumulated::FAccumulated(I, 1);
    return;
}

FAccumulated::FAccumulated(const int V, const uint16_t A)
{
    this->Item = EItem::NullItem;
    this->Voxel = V;
    this->Amount = A;

    return;
}

FAccumulated::FAccumulated(const EItem I, const uint16_t A)
{
    this->Item = I;
    this->Voxel = EWorldVoxel::VoxelNull;
    this->Amount = A;

    return;
}

FAccumulated::FAccumulated(const EItem I, const int V)
{
    this->Item = I;
    this->Voxel = V;
    this->Amount = 1;

    return;
}

FAccumulated::FAccumulated(const int V, const EItem I, const uint16_t A)
{
    this->Item = I;
    this->Voxel = V;
    this->Amount = A;

    return;
}

FString FAccumulated::ToString() const
{
    return FString::Printf(TEXT("FAccumulated { Voxel: %d, Item: %s, Amount: %d }"), this->Voxel, *UEnum::GetValueAsString(this->Item), this->Amount);
}
