// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ChunkStates.h"
#include "CommonChunk.h"
#include "Containers/MyQueue.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

class UDebugScreen;
class ULocalChunkWorldSettings;
class UServerChunkWorldSettings;

struct FClientChunk
{
    FChunkKey                           ChunkKey;
    TFunction<void(voxel_t* VoxelData)> Callback;
};

/** Loads and unloads chunk into / from the UWorld based on what a current validation subsystem has determined. */
UCLASS(NotBlueprintable)
class JAFG_API UChunkGenerationSubsystem final : public UJAFGTickableWorldSubsystemNoDev
{
    GENERATED_BODY()

    friend UDebugScreen;

public:

    UChunkGenerationSubsystem();

    // UWorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~UWorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkGenerationSubsystem, STATGROUP_Tickables)
    }
    // ~FTickableGameObject implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation

    FORCEINLINE auto IsReady(void) const -> bool { return this->bHasReceivedReplicatedServerChunkWorldSettings; }

    FORCEINLINE auto GenerateVerticalChunkAsync(const FChunkKey2& ChunkKey) -> void { this->VerticalChunkQueue.Enqueue(ChunkKey); }
    FORCEINLINE auto GetVerticalChunkQueue(void) -> const TMyQueue<FChunkKey2>& { return this->VerticalChunkQueue; }
    FORCEINLINE auto ClearVerticalChunkQueue(void) -> void { this->VerticalChunkQueue.Empty(); }

                auto AddVerticalChunkToPendingKillQueue(const FChunkKey2& ChunkKey) -> void;
    FORCEINLINE auto GetPendingKillVerticalChunkQueue(void) const -> const TMyQueue<FChunkKey2>& { return this->PendingKillVerticalChunkQueue; }

    FORCEINLINE auto GetVerticalChunks(void) const -> const TSet<FChunkKey2>& { return this->VerticalChunks; }
    FORCEINLINE auto GetPersistentVerticalChunks(void) const -> TArray<FChunkKey2>
    {
        TArray<FChunkKey2> Out;
        for (const FChunkKey2& ChunkKey : this->VerticalChunks)
        {
            if (this->ChunkMap[FChunkKey(ChunkKey.X, ChunkKey.Y, 0)]->GetChunkPersistency() == EChunkPersistency::Persistent)
            {
                Out.Add(ChunkKey);
            }
        }
        return Out;
    }

    [[nodiscard]] FORCEINLINE auto HasPersistentVerticalChunk(const FChunkKey2& ChunkKey) const -> bool
    {
        if (const TObjectPtr<ACommonChunk>* T = this->ChunkMap.Find(FChunkKey(ChunkKey.X, ChunkKey.Y, 0)); T)
        { return (*T)->GetChunkPersistency() == EChunkPersistency::Persistent; } return false;
    }

    [[nodiscard]] FORCEINLINE auto FindChunkByKey(const FChunkKey& ChunkKey) const -> ACommonChunk*
    { if (const TObjectPtr<ACommonChunk>* T = this->ChunkMap.Find(ChunkKey); T) { return *T; } return nullptr; }

    FORCEINLINE auto AddClientChunk(const FClientChunk& ClientChunk) -> void { this->ClientQueue.Enqueue(ClientChunk); }

    FORCEINLINE auto SetInitializationDataFromAuthority(const FChunkKey& ChunkKey, voxel_t* Voxels) -> void
    {
        this->ChunkMap[ChunkKey]->SetInitializationDataFromAuthority(Voxels);
    }

    /** If the chunk itself decided to kill itself. */
    FORCEINLINE void OnChunkWasKilledExternally(const FChunkKey ChunkKey)
    {
        this->ChunkMap.Remove(ChunkKey);
        this->VerticalChunks.Remove(FChunkKey2(ChunkKey.X, ChunkKey.Y));
    }

    /** @return True, if OutLocation is meaningful. */
    auto FindAppropriateLocationForCharacterSpawn(const FVector2D& InApproximateLocation, FVector& OutLocation) const -> bool;
    /** @return True, if OutLocation is meaningful. */
    auto FindAppropriateLocationForCharacterSpawn(const FVector& InApproximateLocation, FVector& OutLocation) const -> bool;

private:

    const float ChunkGenerationInterval            = 0.1f;
    const int32 MaxVerticalChunksToGeneratePerTick = 20;
    const int32 MaxVerticalChunksToAnswerPerTick   = 40;
    /** True if this subsystem was spawned in a client UWorld. */
          bool  bInClientMode                      = false;

    bool bHasReceivedReplicatedServerChunkWorldSettings = false;
    /** Copied so we do not have to deal with checking the local and server config every time. */
    int CopiedChunksAboveZero = -1;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<UServerChunkWorldSettings> ServerChunkWorldSettings;

    TMyQueue<FChunkKey2> VerticalChunkQueue;
    auto DequeueNextVerticalChunk(void) -> void;
    /** Called on the client will handle everything (e.g.: network replication). */
    auto SafeLoadClientVerticalChunkAsync(const TArray<FChunkKey>& Chunks) -> void;
    auto SafeLoadVerticalChunk(
        const TArray<FChunkKey>& Chunks,
        const bool bGenerateMesh = true,
        const EChunkPersistency::Type Persistency = EChunkPersistency::Persistent,
        const float TimeToLive = 10.0f,
        const EChunkState::Type TargetState = EChunkState::Active
    ) -> void;

    TMyQueue<FClientChunk> ClientQueue;
    /** Called on the server to fulfill the requests of the clients. */
    auto DequeueNextClientChunk(void) -> void;

    TMyQueue<FChunkKey2> PendingKillVerticalChunkQueue;
    auto DequeueNextVerticalChunkToKill(void) -> void;

    /** Chunks that have a counterpart in the UWorld. */
    TMap<FChunkKey, TObjectPtr<ACommonChunk>> ChunkMap;
    TSet<FChunkKey2> VerticalChunks;

    /** Spawns a chunk in the EChunkState#PreSpawned state. */
    auto SpawnChunk(const FChunkKey& ChunkKey) const -> ACommonChunk*;

    //////////////////////////////////////////////////////////////////////////
    // Helper methods.
    //////////////////////////////////////////////////////////////////////////

    void PrepareWorldForChunkTransit_Spawned(const FChunkKey2& Chunk);
    void PrepareWorldForChunkTransit_Shaped(const FChunkKey2& Chunk);
    void PrepareWorldForChunkTransit_SurfaceReplaced(const FChunkKey2& Chunk);

    //////////////////////////////////////////////////////////////////////////
    // Useful common getters.
    //////////////////////////////////////////////////////////////////////////

    /** @param Out Order is from bottom to top. */
    auto GetAllChunksFromVerticalChunk(const FChunkKey2& ChunkKey, TArray<FChunkKey>& Out) const -> void;
    auto GetAllChunksFromVerticalChunk(const FChunkKey2& ChunkKey) const -> TArray<FChunkKey>;
    /** @param Out Order is from top to bottom. */
    auto GetAllChunksFromVerticalChunkReversed(const FChunkKey2& ChunkKey, TArray<FChunkKey>& Out) const -> void;
};
