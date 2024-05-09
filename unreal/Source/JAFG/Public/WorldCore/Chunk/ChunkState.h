// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM()
namespace EChunkState
{

enum Type
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

    Shaped,
    SurfaceReplaced,

    Active,
    PendingKill,
};

}

namespace EChunkState
{

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
    default:
    {
        checkNoEntry()
        LOG_FATAL(LogChunkGeneration, "Unknown chunk state %d.", ChunkState)
        return TEXT("Unknown");
    }
    }
}

}
