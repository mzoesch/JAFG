// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGeneratorInfo.h"

#include "Network/NetworkStatics.h"
#include "World/Chunk/GreedyChunk.h"
#include "World/Chunk/ChunkWorldSubsystem.h"

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

    check( this->GetWorldSettings() )
    check( Cast<AChunkWorldSettings>(this->GetWorldSettings()) )

    if (Cast<AChunkWorldSettings>(this->GetWorldSettings())->bOverrideServerChunkGenerationTickRate)
    {
        this->PrimaryActorTick.TickInterval = Cast<AChunkWorldSettings>(this->GetWorldSettings())->ServerChunkGenerationTickRate;
    }

    return;
}

void AWorldGeneratorInfo::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->ChunkGenerationQueue.IsEmpty())
    {
        return;
    }

    check( this->GetWorldSettings() )
    check( Cast<AChunkWorldSettings>(this->GetWorldSettings()) )

    for (int i = 0; i < Cast<AChunkWorldSettings>(this->GetWorldSettings())->MaxServerChunksPerTick; ++i)
    {
        FIntVector Key;
        if (this->ChunkGenerationQueue.Dequeue(Key) == false)
        {
            UE_LOG(LogTemp, Verbose, TEXT("AWorldGeneratorInfo::Tick(): Dequeue failed. World generation is complete for this tick."))
            return;
        }

        const FTransform TargetedChunkTransform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                Key.X * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale,
                Key.Y * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale,
                Key.Z * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale
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
