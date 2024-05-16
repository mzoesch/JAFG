// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "System/TypeDefs.h"
#include "WorldCore/WorldTypes.h"

namespace ChunkStatics
{

/** Transforms an unreal vector to a chunk key. */
FORCEINLINE FChunkKey WorldToChunkKey(const FVector& WorldLocation)
{
    constexpr double Factor { WorldStatics::ChunkSize * WorldStatics::JToUScaleDouble };

    FChunkKey ChunkKey;

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

    if (WorldLocation.Z < 0)
    {
        ChunkKey.Z = static_cast<int>(WorldLocation.Z / Factor) - 1;
    }
    else
    {
        ChunkKey.Z = static_cast<int>(WorldLocation.Z / Factor);
    }

    return ChunkKey;
}

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
