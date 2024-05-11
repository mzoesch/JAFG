// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

class ACommonChunk;
class ULocalChunkWorldSettings;

/** Loads and unloads chunk into / from the UWorld based on what a current validation subsystem has determined. */
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

    FORCEINLINE auto GenerateVerticalChunkAsync(const FChunkKey2& ChunkKey) -> void { this->VerticalChunkQueue.Enqueue(ChunkKey); }
    FORCEINLINE auto GetVerticalChunkQueue(void) -> const TQueue<FChunkKey2>& { return this->VerticalChunkQueue; }
    FORCEINLINE auto ClearVerticalChunkQueue(void) -> void { this->VerticalChunkQueue.Empty(); }

                auto AddVerticalChunkToPendingKillQueue(const FChunkKey2& ChunkKey) -> void;
    FORCEINLINE auto GetPendingKillVerticalChunkQueue(void) -> const TQueue<FChunkKey2>& { return this->PendingKillVerticalChunkQueue; }

    FORCEINLINE auto GetVerticalChunks(void) const -> const TSet<FChunkKey2>& { return this->VerticalChunks; }

private:

    const float ChunkGenerationInterval            = 0.1f;
    const int32 MaxVerticalChunksToGeneratePerTick = 20;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    TQueue<FChunkKey2> VerticalChunkQueue;
    auto DequeueNextVerticalChunk(void) -> void;
    auto SafeLoadVerticalChunk(const TArray<FChunkKey>& Chunks) -> void;

    TQueue<FChunkKey2> PendingKillVerticalChunkQueue;
    auto DequeueNextVerticalChunkToKill(void) -> void;

    /** Chunks that have a counterpart in the UWorld. */
    TMap<FChunkKey, TObjectPtr<ACommonChunk>> ChunkMap;
    TSet<FChunkKey2> VerticalChunks;

    /** Spawns a chunk in the EChunkState#PreSpawned state. */
    auto SpawnChunk(const FChunkKey& ChunkKey) const -> ACommonChunk*;
};
