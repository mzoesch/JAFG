// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define FChunkKey FIntVector

// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define FVoxelKey FIntVector

namespace ETextureGroup
{

enum Type : int8
{
    Core = -1,
    Opaque,
    FullBlendOpaque,
    FloraBlendOpaque,
};

}

namespace ENormalLookup
{

enum Type : int8
{
    Default,
    Top,
    Bottom,
    Front,
    Side,
};

FORCEINLINE ENormalLookup::Type FromVector(const FVector& Normal)
{
    if (Normal == FVector::UpVector)
    {
        return ENormalLookup::Top;
    }

    if (Normal == FVector::DownVector)
    {
        return ENormalLookup::Bottom;
    }

    if (Normal == FVector::ForwardVector || Normal == FVector::BackwardVector)
    {
        return ENormalLookup::Front;
    }

    if (Normal == FVector::RightVector || Normal == FVector::LeftVector)
    {
        return ENormalLookup::Side;
    }

    checkNoEntry()
    return ENormalLookup::Default;
};

}
