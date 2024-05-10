// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

class ACommonChunk;
class ULocalChunkWorldSettings;
class UChunkValidationSubsystem;
class UChunkValidationSubsystemDedSv;

/** Loads and unloads chunk into / from the UWorld based on what a current validation subsystem has determined. */
UCLASS(NotBlueprintable)
class JAFG_API UChunkGenerationSubsystem final : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

    friend UChunkValidationSubsystem;
    friend UChunkValidationSubsystemDedSv;

public:

    UChunkGenerationSubsystem();

    // UWorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~UWorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkGenerationSubsystem, STATGROUP_Tickables);
    }
    // ~FTickableGameObject implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation

    /**
     * Spawns these chunks at an unknown time in the future.
     * These chunks are going to be active chunks. Meaning all other chunks that these chunks depend on are going to be
     * spawned as well and set to their appropriate states.
     */
    void SpawnActiveVerticalChunkAsync(const FChunkKey2& VerticalChunkKey);

private:

    const float ChunkGenerationInterval    = 0.1f;
    const int32 MaxChunksToGeneratePerTick = 50;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    /** Chunks added to this queue will be generated as soon as possible asynchronously. */
    TQueue<FChunkKey>  ActiveChunksToGenerateAsyncQueue;
    TQueue<FChunkKey2> ActiveVerticalChunksToGenerateAsyncQueue;

    /** Chunks that have a counterpart in the UWorld. */
    TMap<FChunkKey, TObjectPtr<ACommonChunk>> ChunkMap;

    TSet<FChunkKey2> ActiveVerticalChunks;
    FORCEINLINE auto IsVerticalChunkActive(const FChunkKey2& ChunkKey) const -> bool { return this->ActiveVerticalChunks.Contains(ChunkKey); }

    void DequeueAllActiveVerticalChunks(void);
    void DequeueNextActiveVerticalChunk(void);
    /**
     * Dequeues the next chunk from the UChunkGenerationSubsystem#ActiveChunksToGenerateAsyncQueue and spawns it
     * as well all other chunks that this chunk depends on.
     */
    void DequeueNextActiveChunk(void);

    /** Low-level implementation of spawning a chunk. Never call directly. */
    ACommonChunk* SpawnChunk(const FChunkKey& ChunkKey) const;
};
