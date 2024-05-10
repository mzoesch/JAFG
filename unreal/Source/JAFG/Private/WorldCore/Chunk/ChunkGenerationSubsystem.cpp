// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/CommonChunk.h"
#include "WorldCore/Chunk/GreedyChunk.h"
#include "WorldCore/Chunk/ChunkStates.h"

UChunkGenerationSubsystem::UChunkGenerationSubsystem(void) : Super()
{
    return;
}

void UChunkGenerationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Super::Initialize(Collection);

    this->SetTickInterval(this->ChunkGenerationInterval);

    return;
}

void UChunkGenerationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ActiveChunksToGenerateAsyncQueue.Empty();
    this->ActiveVerticalChunksToGenerateAsyncQueue.Empty();
    this->ChunkMap.Empty();
    this->ActiveVerticalChunks.Empty();

    this->LocalChunkWorldSettings = this->GetWorld()->GetSubsystem<ULocalChunkWorldSettings>();
    check( this->LocalChunkWorldSettings )

    return;
}

void UChunkGenerationSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    if (this->ActiveVerticalChunksToGenerateAsyncQueue.IsEmpty() == false)
    {
        this->DequeueAllActiveVerticalChunks();
    }

    /* Early exit if there are no chunks to generate. */
    if (this->ActiveChunksToGenerateAsyncQueue.IsEmpty())
    {
        return;
    }

    int32 ChunksGenerated = 0;
    while (ChunksGenerated < this->MaxChunksToGeneratePerTick && this->ActiveChunksToGenerateAsyncQueue.IsEmpty() == false)
    {
        this->DequeueNextActiveChunk();
        ChunksGenerated++;
    }

    return;
}

void UChunkGenerationSubsystem::SpawnActiveVerticalChunkAsync(const FIntVector2& VerticalChunkKey)
{
    this->ActiveVerticalChunksToGenerateAsyncQueue.Enqueue(VerticalChunkKey);
}

void UChunkGenerationSubsystem::DequeueAllActiveVerticalChunks(void)
{
    while (this->ActiveVerticalChunksToGenerateAsyncQueue.IsEmpty() == false)
    {
        this->DequeueNextActiveVerticalChunk();
    }

    return;
}

void UChunkGenerationSubsystem::DequeueNextActiveVerticalChunk(void)
{
    FIntVector2 NewActiveKey;
    if (this->ActiveVerticalChunksToGenerateAsyncQueue.Dequeue(NewActiveKey) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    for (int32 Z = 0; Z < this->LocalChunkWorldSettings->ReplicatedChunksAboveZero; ++Z)
    {
        FChunkKey NewActiveChunkKey = FChunkKey(NewActiveKey.X, NewActiveKey.Y, Z);
        this->ActiveChunksToGenerateAsyncQueue.Enqueue(NewActiveChunkKey);
    }

    this->ActiveVerticalChunks.Add(NewActiveKey);

    return;
}

void UChunkGenerationSubsystem::DequeueNextActiveChunk(void)
{
    FChunkKey NewActiveKey;
    if (this->ActiveChunksToGenerateAsyncQueue.Dequeue(NewActiveKey) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    ACommonChunk* NewActiveChunk;
    if (this->ChunkMap.Contains(NewActiveKey) == false)
    {
        NewActiveChunk = this->SpawnChunk(NewActiveKey);
        this->ChunkMap.Add(NewActiveKey, NewActiveChunk);

    }
    else
    {
        NewActiveChunk = this->ChunkMap[NewActiveKey];
    }

    NewActiveChunk->SetChunkState(EChunkState::Spawned);
    NewActiveChunk->SetChunkState(EChunkState::Shaped);
    NewActiveChunk->SetChunkState(EChunkState::SurfaceReplaced);
    NewActiveChunk->SetChunkState(EChunkState::Active);

    return;
}

ACommonChunk* UChunkGenerationSubsystem::SpawnChunk(const FChunkKey& ChunkKey) const
{
#if LOG_PERFORMANCE_CRITICAL_SECTIONS
    LOG_VERY_VERBOSE(LogChunkGeneration, "Spawning chunk at %s.", *ChunkKey.ToString())
#endif /* LOG_PERFORMANCE_CRITICAL_SECTIONS */

    const FTransform TargetedChunkTransform = FTransform(
        FRotator::ZeroRotator,
        FVector(
            ChunkKey.X * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            ChunkKey.Y * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            ChunkKey.Z * WorldStatics::ChunkSize * WorldStatics::JToUScale
        ),
        FVector::OneVector
    );

    ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(
        this->LocalChunkWorldSettings->LocalChunkType == EChunkType::Greedy
        ? AGreedyChunk::StaticClass() : ACommonChunk::StaticClass(),
        TargetedChunkTransform
    );

    return Chunk;
}
