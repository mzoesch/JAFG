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
    // All other groups are determined at runtime
};

FORCEINLINE ETextureGroup::Type FromBlendArrIdx(const int32 BlendArrIdx)
{
    return static_cast<ETextureGroup::Type>(BlendArrIdx + 1);
}

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

FORCEINLINE bool IsValid(const FString& Normal)
{
    return Normal == TEXT("Top") || Normal == TEXT("Bot") || Normal == TEXT("Front") || Normal == TEXT("Side");
}

FORCEINLINE ENormalLookup::Type FromString(const FString& Normal)
{
    if (Normal == TEXT("Top"))
    {
        return ENormalLookup::Top;
    }

    if (Normal == TEXT("Bot"))
    {
        return ENormalLookup::Bottom;
    }

    if (Normal == TEXT("Front"))
    {
        return ENormalLookup::Front;
    }

    if (Normal == TEXT("Side"))
    {
        return ENormalLookup::Side;
    }

    checkNoEntry()
    return ENormalLookup::Default;
}

}
