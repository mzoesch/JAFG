// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"

JAFG_VOID

namespace EChunkState
{

enum Type : uint8
{
    /**
     * Marks an invalid state. A chunk must never have this state.
     */
    Invalid,

    /**
     * The chunk exists in the UWorld but is free und unbound to any responsibilities. A chunk must never be
     * interacted with when in this state.
     * The ACommonChunk#BeginPlay method has been called.
     */
    Freed,

    /**
     * A chunk is in the UWorld present with its most minimal variables set.
     * This state requires no more conditions than the previous state.
     */
    PreSpawned,

    /**
     * A chunk has run its ACommonChunk#InitializeCommonStuff method.
     * It now knows its neighbors, and some essential variables have been set.
     * A spawned chunk requires that all six of its neighbors exist and have at least
     * the state EChunkState::PreSpawned.
     */
    Spawned,

    /**
     * The chunk has been shaped. The chunk-raw-data must now consist of two different voxels.
     * The air voxel and the base voxel.
     * This state requires no more conditions than the previous state.
     */
    Shaped,

    /**
     * The surface voxels of the chunk have been replaced based on the neighboring chunks.
     * This state requires that all six of its neighbors have at least the state EChunkState::Shaped.
     */
    SurfaceReplaced,

    /**
     * Caves have now been generated in the chunk.
     * This state requires no more conditions than the previous state.
     */
    ShapedCaves,

    /**
     * The chunk is now complete-active, and the local player can interact with it.
     * This is the first time that the mesh of the chunks is actually generated.
     * This state requires no more conditions than the previous state.
     */
    Active,

    /*----------------------------------------------------------------------------
        Special states below here.
        These entries have nothing to do with generation and are considered
        special as they will not interfere with the subsystem.
        A chunk must never have this state as it is only used to classify
        states together - like a barrier.
    ----------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------
    ----------------------------------------------------------------------------*/

    Special,

    /*----------------------------------------------------------------------------
    ----------------------------------------------------------------------------*/

    /**
     * Chunk has been marked as a pending kill and is no longer visible to the player and the UWorld (the mesh
     * has been cleared to save memory).
     * The validation system will remove this chunk from the UWorld in the near future.
     */
    PendingKill,

    /**
     * If the chunk itself decides that it wants to die and be separated from all its friends in the UWorld.
     * The corresponding AActor will probably be killed at the end of the tick, where the state change was broadcasted.
     */
    Kill,

    /**
     * This chunk cannot progress to the next or any other state until the hyperlane has answered this chunk's request
     * to be generated.
     * The only exception is the state EChunkState::PendingKill.
     */
    BlockedByHyperlane,
};

}

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
    case EChunkState::ShapedCaves:
    {
        return TEXT("ShapedCaves");
    }
    case EChunkState::Active:
    {
        return TEXT("Active");
    }
    case EChunkState::Special:
    {
        return TEXT("Special");
    }
    case EChunkState::PendingKill:
    {
        return TEXT("PendingKill");
    }
    case EChunkState::Kill:
    {
        return TEXT("Kill");
    }
    case EChunkState::BlockedByHyperlane:
    {
        return TEXT("BlockedByHyperlane");
    }
    default:
    {
        jcheckNoEntry()
        return TEXT("Unknown");
    }
    }
}

namespace EChunkPersistency
{

enum Type : uint8
{
    Persistent,
    Temporary,
};

}

FORCEINLINE auto LexToString(const EChunkPersistency::Type ChunkPersistency) -> FString
{
    switch (ChunkPersistency)
    {
    case EChunkPersistency::Persistent:
    {
        return TEXT("Persistent");
    }
    case EChunkPersistency::Temporary:
    {
        return TEXT("Temporary");
    }
    default:
    {
        jcheckNoEntry()
        return TEXT("Unknown");
    }
    }
}
