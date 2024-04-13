// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "WorldGeneratorInfo.generated.h"

class ACommonChunk;

DECLARE_MULTICAST_DELEGATE_OneParam(FChunkEventSignature, ACommonChunk* /* Chunk */);

/**
 * Singleton information non-replicated boundless super AActor class sitting in the UWorld responsible for generating
 * the world. It handles the creation and destroying of AChunks depending on the Player States data. Not responsible
 * for the actual World Generation as this is done by the AChunk AActors themselves.
 * A blueprint class may derive from this class to allow for fast UProperty changes during development or prototyping.
 * Not intended to be used by the Kismet system. There are no public methods made available for them.
 */
UCLASS(Blueprintable)
class JAFG_API AWorldGeneratorInfo : public AInfo
{
    GENERATED_BODY()

    friend ACommonChunk;

public:

    explicit AWorldGeneratorInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(const float DeltaTime) override;

private:

    UPROPERTY()
    TMap<FIntVector, ACommonChunk*> FullyLoadedChunks;
    /**
     * The requested chunks, that a client wants to load into the world.
     */
    TQueue<FIntVector> ChunkGenerationQueue;

public:

    FChunkEventSignature OnChunkFinishedGeneratingDelegate;

    FORCEINLINE bool HasFullyLoadedChunk(const FIntVector& ChunkKey) const
    {
        return this->FullyLoadedChunks.Contains(ChunkKey);
    }

    FORCEINLINE bool HasFullyLoadedChunk(const FIntVector& ChunkKey, ACommonChunk*& OutChunk) const
    {
        return this->FullyLoadedChunks.Contains(ChunkKey) ? (OutChunk = this->FullyLoadedChunks[ChunkKey], true) : false;
    }

    FORCEINLINE bool AddChunkToGenerationQueue(const FIntVector& ChunkKey)
    {
        return this->ChunkGenerationQueue.Enqueue(ChunkKey);
    }

    FORCEINLINE ACommonChunk* GetChunkByKey(const FIntVector& ChunkKey) const
    {
        return this->FullyLoadedChunks.Contains(ChunkKey) ? this->FullyLoadedChunks[ChunkKey] : nullptr;
    }
};
