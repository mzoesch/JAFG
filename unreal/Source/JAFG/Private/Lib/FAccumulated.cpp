#include "Lib/FAccumulated.h"

FAccumulated::FAccumulated()
{
    this->Item = EItem::NullItem;
    this->Voxel = EVoxel::Null;
    this->Amount = 0;

    return;
}

FAccumulated::FAccumulated(const EVoxel V)
{
    this->FAccumulated::FAccumulated(V, 1);
    return;
}

FAccumulated::FAccumulated(const EItem I)
{
    this->FAccumulated::FAccumulated(I, 1);
    return;
}

FAccumulated::FAccumulated(const EVoxel V, const uint16_t A)
{
    this->Item = EItem::NullItem;
    this->Voxel = V;
    this->Amount = A;

    return;
}

FAccumulated::FAccumulated(const EItem I, const uint16_t A)
{
    this->Item = I;
    this->Voxel = EVoxel::Null;
    this->Amount = A;

    return;
}
