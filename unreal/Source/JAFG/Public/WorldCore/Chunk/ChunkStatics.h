// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"
#include "System/TypeMacros.h"
#include "WorldCore/WorldTypes.h"

namespace ChunkConversion
{

/** Transforms an unreal vector to a vertical chunk key. */
FORCEINLINE FChunkKey2 WorldToVerticalChunkKey(const FVector& WorldLocation)
{
    constexpr double Factor { WorldStatics::ChunkSize * WorldStatics::JToUScaleDouble };

    FChunkKey2 ChunkKey;

    if (WorldLocation.X < 0)
    {
        ChunkKey.X = static_cast<int>(WorldLocation.X / Factor) - 1;
    }
    else
    {
        ChunkKey.X = static_cast<int>(WorldLocation.X / Factor);
    }

    if (WorldLocation.Y < 0)
    {
        ChunkKey.Y = static_cast<int>(WorldLocation.Y / Factor) - 1;
    }
    else
    {
        ChunkKey.Y = static_cast<int>(WorldLocation.Y / Factor);
    }

    return ChunkKey;
}

}
