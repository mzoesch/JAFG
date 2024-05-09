// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

class ACommonChunk;
class ULocalChunkWorldSettings;

/**
 * Loads and unloads chunk into / from the UWorld based on what the validation subsystem has determined.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkGenerationSubsystem final : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

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
     * Spawns this chunk at an unknown time in the future.
     * This chunk is going to be an active chunk. Meaning all other chunks that this chunk depends on are going to be
     * spawned as well and set to their appropriate states.
     */
    void SpawnActiveChunkAsync(const FChunkKey& ChunkKey);

private:

    const float ChunkGenerationInterval    = 1.0f;
    const int32 MaxChunksToGeneratePerTick = 500;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    /** Chunks added to this queue will be generated as soon as possible asynchronously. */
    TQueue<FChunkKey> ActiveChunksToGenerateAsyncQueue;
    /** Chunks that have a counterpart in the UWorld. */
    TMap<FChunkKey, TObjectPtr<ACommonChunk>> ChunkMap;

    /**
     * Dequeues the next chunk from the UChunkGenerationSubsystem#ActiveChunksToGenerateAsyncQueue and spawns it
     * as well all other chunks that this chunk depends on.
     */
    void DequeueNextActiveChunk(void);




    ACommonChunk* SpawnChunk(const FChunkKey& ChunkKey) const;
};
