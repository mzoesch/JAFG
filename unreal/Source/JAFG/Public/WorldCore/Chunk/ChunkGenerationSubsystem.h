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

private:

    const float ChunkGenerationInterval            = 0.1f;
    const int32 MaxVerticalChunksToGeneratePerTick = 20;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    /** Never enqueue here directly. */
    TQueue<FChunkKey2> PendingKillVerticalChunks;
    auto AddVerticalChunkToKillQueue(const FChunkKey2& ChunkKey) -> void;
    auto DequeueNextVerticalChunkToKill(void) -> void;

    TQueue<FChunkKey2> ActiveVerticalChunksToGenerateAsyncQueue;
    /** Chunks that have a counterpart in the UWorld. */
    TMap<FChunkKey, TObjectPtr<ACommonChunk>> ChunkMap;
    /** Active vertical chunks. All sub-chunks must be inside the UChunkGenerationSubsystem#ChunkMap. */
    TSet<FChunkKey2> ActiveVerticalChunks;

    void DequeueNextActiveVerticalChunk(void);
    void SafeLoadVerticalChunk(const TArray<FChunkKey>& Chunks);

    /** Low-level implementation of spawning a chunk. Never call directly. */
    ACommonChunk* SpawnChunk(const FChunkKey& ChunkKey) const;
};
