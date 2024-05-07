// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

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

    FORCEINLINE void SpawnChunkAsync(const FChunkKey& ChunkKey)
    {
        this->ChunkToGenerateAsyncQueue.Enqueue(ChunkKey);
    }

private:

    const float ChunkGenerationInterval = 1.0f;

    const int32 MaxChunksToGeneratePerTick = 500;

    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    /**
     * Chunks added to this queue will be generated as soon as possible asynchronously.
     */
    TQueue<FChunkKey> ChunkToGenerateAsyncQueue;

    void SpawnChunk(const FChunkKey& ChunkKey);
    // Only temporary
    friend class UChunkValidationSubsystem;
};
