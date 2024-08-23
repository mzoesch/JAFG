// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkArena.generated.h"

JAFG_VOID

class ACommonChunk;
class UChunkArena;

UINTERFACE()
class UChunkArenaGladiator : public UInterface
{
    GENERATED_BODY()
};

class IChunkArenaGladiator
{
    GENERATED_BODY()

    friend UChunkArena;

protected:

    /**
     * When this gladiator should be in a state where it can be freely used.
     * @note This has nothing to do with the BeginPlay method of an AActor. Chunks might be obliterated without
     *       ever this method being called.
     */
    FORCEINLINE virtual auto OnAllocate(const FChunkKey& InChunkKey) -> void        = 0;
    /**
     * When this gladiator should reset its state to be reusable if the OnAllocate will be called in the future
     * of them.
     */
    FORCEINLINE virtual auto OnFree(void) -> void                                   = 0;
    /**
     * When the arena releases this gladiator, and discards their ownership of them.
     */
    FORCEINLINE virtual auto OnObliterate(void) -> void                             = 0;

public:

    FORCEINLINE virtual auto GetChunkKeyOnTheFly_Gladiator(void) const -> FChunkKey = 0;
    FORCEINLINE virtual auto AsChunk(void) const -> const ACommonChunk*             = 0;
    FORCEINLINE virtual auto AsChunk(void) -> ACommonChunk*                         = 0;
};

/**
 * Helps prevent lag spikes when generating chunks by only resetting the necessary parameters inside Chunk AActors to
 * be reused instead of being destroyed and new ones being spawned inside the UWorld.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkArena : public UJAFGWorldSubsystemNoDev
{
    GENERATED_BODY()

public:

    UChunkArena();

    /**
     * Reserves a number of free chunks. That should now be spawned in the UWorld and should wait for allocation.
     *
     * @param InTotalCount The count that should be equivalent to the number of free chunks after this method has been
     *                     executed.
     * @param InMaxCount   The maximum number of chunks that should now be allocated. Negative values will result in
     *                     the InTotalCount being the maximum.
     */
    auto ReserveGladiators(const int32 InTotalCount, const int32 InMaxCount = -1) -> void;
    /**
     * @param InCount The number of free chunks that should be obliterated. Negative values will result in all free
     *                chunks being obliterated.
     */
    auto ObliterateFreeGladiators(const int32 InCount = -1) -> void;

    auto Alloc(const FChunkKey& InChunkKey, IChunkArenaGladiator*& OutGladiator) -> bool;
    auto Free(IChunkArenaGladiator& InGladiator) -> bool;

    FORCEINLINE auto GetAllocatedChunkCount(void) const -> int32 { return this->AllocatedChunks.Num(); }
    FORCEINLINE auto GetFreeChunkCount(void) const -> int32 { return this->FreeChunks.Num(); }
    FORCEINLINE auto GetTotalChunkCount(void) const -> int32 { return this->GetAllocatedChunkCount() + this->GetFreeChunkCount(); }

private:

    TArray<IChunkArenaGladiator*>          FreeChunks;
    TMap<FChunkKey, IChunkArenaGladiator*> AllocatedChunks;

    auto SpawnChunk(void) const -> IChunkArenaGladiator*;
};
