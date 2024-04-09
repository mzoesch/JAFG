// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "WorldGeneratorInfo.generated.h"

class ACommonChunk;

DECLARE_MULTICAST_DELEGATE_OneParam(FChunkEventSignature, ACommonChunk* /* Chunk */);

UENUM(BlueprintType)
enum class EChunkType
{
    Naive,
    Greedy,
    Marching,
};

UENUM(BlueprintType)
enum class EWorldGenerationType
{
    Default,
    SuperFlat,
};

namespace WorldGeneratorInfo
{

FORCEINLINE FString LexToString(const EWorldGenerationType Type)
{
    switch (Type)
    {
    case EWorldGenerationType::Default:
    {
        return FString(TEXT("Default"));
    }
    case EWorldGenerationType::SuperFlat:
    {
        return FString(TEXT("SuperFlat"));
    }
    default:
    {
        return FString(TEXT("Unknown"));
    }
    }
}

}

// TODO Make this a subsystem
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

private:

    //////////////////////////////////////////////////////////////////////////
    // Note that all these member variables are not replicated.
    // And therefore only valid on a server.
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    EChunkType ChunkType = EChunkType::Greedy;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    EWorldGenerationType WorldGenerationType = EWorldGenerationType::SuperFlat;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    int ChunksAboveZero = 3;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation", meta = (AllowPrivateAccess = "true"))
    int MaxSpiralPoints = 20;

public:

    /**
     * Do not increase this value beyond 16 for now. As we would breach the Bunch size limit of 2^16 = 65.536 bytes.
     * See CommonChunk.h for more information.
     */
    inline static constexpr int ChunkSize { 16 };

    inline static constexpr float  JToUScale { 100.0f };
    inline static constexpr float  UToJScale { 1.0f / AWorldGeneratorInfo::JToUScale };
    inline static constexpr double JToUScaleDouble { 100.0 };
    inline static constexpr double UToJScaleDouble { 1.0 / AWorldGeneratorInfo::JToUScaleDouble };
    inline static constexpr int    JToUScaleInteger { 100 };
    /* There is obviously no integer with a U To J Scale. */

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
    FORCEINLINE bool AddChunkToGenerationQueue(const FIntVector& ChunkKey)
    {
        return this->ChunkGenerationQueue.Enqueue(ChunkKey);
    }

    FORCEINLINE ACommonChunk* GetChunkBy(const FIntVector& ChunkKey) const
    {
        return this->FullyLoadedChunks.Contains(ChunkKey) ? this->FullyLoadedChunks[ChunkKey] : nullptr;
    }
};
