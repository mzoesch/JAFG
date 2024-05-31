// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTypes.h"

namespace ChunkStatics
{

/** Transforms an unreal vector to a chunk key. */
FORCEINLINE auto WorldToChunkKey(const FVector& WorldLocation) -> FChunkKey
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
FORCEINLINE auto WorldToVerticalChunkKey(const FVector2D& WorldLocation) -> FChunkKey2
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

/** Transforms an unreal vector to a vertical chunk key. */
FORCEINLINE auto WorldToVerticalChunkKey(const FVector& WorldLocation) -> FChunkKey2
{
    return ChunkStatics::WorldToVerticalChunkKey(FVector2D(WorldLocation.X, WorldLocation.Y));
}

/** Transforms an unreal vector to a local voxel key. */
FORCEINLINE auto WorldToLocalVoxelLocation(const FVector& WorldLocation) -> FVoxelKey
{
    constexpr double Factor { WorldStatics::ChunkSize * WorldStatics::JToUScaleDouble };

    FIntVector ChunkLocation;

    if (WorldLocation.X < 0)
    {
        ChunkLocation.X = static_cast<int>(WorldLocation.X / Factor) - 1;
    }
    else
    {
        ChunkLocation.X = static_cast<int>(WorldLocation.X / Factor);
    }

    if (WorldLocation.Y < 0)
    {
        ChunkLocation.Y = static_cast<int>(WorldLocation.Y / Factor) - 1;
    }
    else
    {
        ChunkLocation.Y = static_cast<int>(WorldLocation.Y / Factor);
    }

    if (WorldLocation.Z < 0)
    {
        ChunkLocation.Z = static_cast<int>(WorldLocation.Z / Factor) - 1;
    }
    else
    {
        ChunkLocation.Z = static_cast<int>(WorldLocation.Z / Factor);
    }

    FIntVector LocalVoxelLocation =
          FIntVector(WorldLocation) / WorldStatics::JToUScaleInteger
        - ChunkLocation * WorldStatics::ChunkSize;

    /* Negative Normalization */

    if (ChunkLocation.X < 0)
    {
        --LocalVoxelLocation.X;
    }

    if (ChunkLocation.Y < 0)
    {
        --LocalVoxelLocation.Y;
    }

    if (ChunkLocation.Z < 0)
    {
        --LocalVoxelLocation.Z;
    }

    return LocalVoxelLocation;
}

/** Transforms an unreal vector to a j coordinate. */
FORCEINLINE auto WorldToJCoordinate(const FVector& WorldLocation) -> FJCoordinate
{
    return FJCoordinate(WorldLocation) * WorldStatics::UToJScale;
}

/** Transforms an unreal vector to a vertical j coordinate. */
FORCEINLINE auto WorldToVerticalJCoordinate(const FVector2D& WorldLocation) -> FJCoordinate2
{
    return FJCoordinate2(WorldLocation.X, WorldLocation.Y) * WorldStatics::UToJScale;
}

/** Transforms an unreal vector to a vertical j coordinate. */
FORCEINLINE auto WorldToVerticalJCoordinate(const FVector& WorldLocation) -> FJCoordinate2
{
    return FJCoordinate2(WorldLocation.X, WorldLocation.Y) * WorldStatics::UToJScale;
}

FORCEINLINE auto JCoordinateToWorldLocation(const FJCoordinate& JCoordinate) -> FVector
{
    return FVector(JCoordinate) * WorldStatics::JToUScale;
}

}
