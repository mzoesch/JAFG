// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGeneratorInfo.h"

#include "Network/NetworkStatics.h"
#include "World/Chunk/GreedyChunk.h"

AWorldGeneratorInfo::AWorldGeneratorInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
    this->PrimaryActorTick.bStartWithTickEnabled = true;
    this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    this->PrimaryActorTick.TickInterval = 1.0f;

    this->bReplicates = false;
    this->bNetLoadOnClient = false;

    this->FullyLoadedChunks = TMap<FIntVector, ACommonChunk*>();
    this->ChunkGenerationQueue.Empty();

    return;
}

void AWorldGeneratorInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this))
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldGeneratorInfo::BeginPlay: Running on a client. This is disallowed."))
        return;
    }

    this->FullyLoadedChunks.Empty();
    this->ChunkGenerationQueue.Empty();

    /*
     * The server will generate the chunks based on the needs of a client.
     */

    return;
}

void AWorldGeneratorInfo::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->ChunkGenerationQueue.IsEmpty())
    {
        return;
    }

    constexpr int MaxNewChunksPerTick = 100;
    for (int i = 0; i < MaxNewChunksPerTick; ++i)
    {
        FIntVector Key;
        if (this->ChunkGenerationQueue.Dequeue(Key) == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::Tick(): Dequeue failed. World generation is complete for this tick."))
            return;
        }

        const FTransform TargetedChunkTransform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                Key.X * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                Key.Y * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                Key.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale
            ),
            FVector::OneVector
        );

        ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);

        /* We of course have to first add this to the sweep steps later. */
        this->FullyLoadedChunks.Add(Key, Chunk);

        this->OnChunkFinishedGeneratingDelegate.Broadcast(Chunk);

        continue;
    }

    return;
}
