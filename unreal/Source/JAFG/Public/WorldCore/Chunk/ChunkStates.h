// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"

namespace EChunkState
{

enum Type : uint8
{
    /**
     * Marks an invalid state. A chunk must never have this state.
     */
    Invalid,

    /**
     * A chunk is in the UWorld present with its most minimal variables set.
     * The ACommonChunk#BeginPlay method has been called.
     * It does not require any additional things to be spawned or set.
     */
    PreSpawned,

    /**
     * A chunk has run its ACommonChunk#PreInitialize method.
     * It now knows its neighbors, and some essential variables have been set.
     * A spawned chunk requires that all six of its neighbors exist and have at least
     * the state EChunkState::PreSpawned.
     */
    Spawned,

    /**
     * The chunk has been shaped. The chunk-raw-data must now consist of two different voxels.
     * The air voxel and the base voxel.
     */
    Shaped,

    /**
     * The surface voxels of the chunk have been replaced based on the neighboring chunks.
     * This state requires that all six of its neighbors have at least the state EChunkState::Shaped.
     */
    SurfaceReplaced,

    /**
     * The chunk is now complete-active, and the local player can interact with it.
     * This is the first time that the mesh of the chunks is actually generated.
     */
    Active,

    /**
     * Chunk has been marked as a pending kill and is no longer visible to the player and the UWorld (the mesh
     * has been cleared to save memory).
     * The validation system will remove this chunk from the UWorld in the near future.
     */
    PendingKill,

    /**
     * This chunk cannot progress to the next or any other state until the hyperlane has answered this chunk's request
     * to be generated.
     * The only exception is the state EChunkState::PendingKill.
     */
    BlockedByHyperlane,
};

FORCEINLINE auto LexToString(const EChunkState::Type ChunkState) -> FString
{
    switch (ChunkState)
    {
    case EChunkState::Invalid:
    {
        return TEXT("Invalid");
    }
    case EChunkState::PreSpawned:
    {
        return TEXT("PreSpawned");
    }
    case EChunkState::Spawned:
    {
        return TEXT("Spawned");
    }
    case EChunkState::Shaped:
    {
        return TEXT("Shaped");
    }
    case EChunkState::SurfaceReplaced:
    {
        return TEXT("SurfaceReplaced");
    }
    case EChunkState::Active:
    {
        return TEXT("Active");
    }
    case EChunkState::PendingKill:
    {
        return TEXT("PendingKill");
    }
    case EChunkState::BlockedByHyperlane:
    {
        return TEXT("BlockedByHyperlane");
    }
    default:
    {
        checkNoEntry()
        LOG_FATAL(LogChunkGeneration, "Unknown chunk state %d.", ChunkState)
        return TEXT("Unknown");
    }
    }
}

}
