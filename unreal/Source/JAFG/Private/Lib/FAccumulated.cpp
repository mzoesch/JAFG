// © 2023 mzoesch. All rights reserved.

#include "Lib/FAccumulated.h"

const FAccumulated FAccumulated::NullAccumulated = FAccumulated(EVoxel::Null, EItem::NullItem, 0);

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

FAccumulated::FAccumulated(const EItem I, const EVoxel V)
{
    this->Item = I;
    this->Voxel = V;
    this->Amount = 1;

    return;
}

FAccumulated::FAccumulated(const EVoxel V, const EItem I, const uint16_t A)
{
    this->Item = I;
    this->Voxel = V;
    this->Amount = A;

    return;
}

FString FAccumulated::ToString() const
{
    return FString::Printf(TEXT("FAccumulated { Voxel: %s, Item: %s, Amount: %d }"), *UEnum::GetValueAsString(this->Voxel), *UEnum::GetValueAsString(this->Item), this->Amount);
}
