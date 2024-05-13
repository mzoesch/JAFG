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
    this->VerticalChunkQueue.Empty();
    this->ChunkMap.Empty();
    this->VerticalChunks.Empty();

    if (UNetStatics::IsSafeDedicatedServer(&InWorld) == false)
    {
        this->LocalChunkWorldSettings = InWorld.GetSubsystem<ULocalChunkWorldSettings>();
        check( this->LocalChunkWorldSettings )
    }

    if (UNetStatics::IsSafeServer(&InWorld))
    {
        this->ServerChunkWorldSettings = InWorld.GetSubsystem<UServerChunkWorldSettings>();
        check( this->ServerChunkWorldSettings )
    }

    if (UNetStatics::IsSafeClient(&InWorld))
    {
        this->bInClientMode = true;
    }

    this->CopiedChunksAboveZero =
        this->LocalChunkWorldSettings
            ? this->LocalChunkWorldSettings->ReplicatedChunksAboveZero
            : this->ServerChunkWorldSettings->ChunksAboveZero;

    this->PendingKillVerticalChunkQueue.Enqueue(FChunkKey2(0, 0));
    this->PendingKillVerticalChunkQueue.Empty();

    return;
}

void UChunkGenerationSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    // Unloading
    //////////////////////////////////////////////////////////////////////////
    while (this->PendingKillVerticalChunkQueue.IsEmpty() == false)
    {
        this->DequeueNextVerticalChunkToKill();
    }

    // Loading
    //////////////////////////////////////////////////////////////////////////
    int32 ChunksGenerated = 0;
    while (
           ChunksGenerated < this->MaxVerticalChunksToGeneratePerTick
        && this->VerticalChunkQueue.IsEmpty() == false
    )
    {
        this->DequeueNextVerticalChunk();
        ChunksGenerated++;
    }

    return;
}

void UChunkGenerationSubsystem::AddVerticalChunkToPendingKillQueue(const FChunkKey2& ChunkKey)
{
    this->PendingKillVerticalChunkQueue.Enqueue(ChunkKey);
    for (int32 Z = 0; Z < this->CopiedChunksAboveZero; ++Z)
    {
        this->ChunkMap[FChunkKey(ChunkKey.X, ChunkKey.Y, Z)]->SetChunkState(EChunkState::PendingKill);
    }


    return;
}

void UChunkGenerationSubsystem::DequeueNextVerticalChunk(void)
{
    FIntVector2 NewActiveKey;
    if (this->VerticalChunkQueue.Dequeue(NewActiveKey) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    TArray<FChunkKey> NewChunks = TArray<FChunkKey>();
    NewChunks.Reserve(this->CopiedChunksAboveZero);

    for (int32 Z = 0; Z < this->CopiedChunksAboveZero; ++Z)
    {
        NewChunks.Add(FChunkKey(NewActiveKey.X, NewActiveKey.Y, Z));
    }

    this->VerticalChunks.Add(NewActiveKey);

    if (this->bInClientMode)
    {
        this->SafeLoadClientVerticalChunkAsync(NewChunks);
    }
    else
    {
        this->SafeLoadVerticalChunk(NewChunks);
    }

    return;
}

void UChunkGenerationSubsystem::SafeLoadClientVerticalChunkAsync(const TArray<FChunkKey>& Chunks)
{
    for (const FChunkKey& Chunk : Chunks)
    {
        const TObjectPtr<ACommonChunk>* MapPtr = this->ChunkMap.Find(Chunk);
        ACommonChunk* ChunkPtr;
        if (MapPtr == nullptr)
        {
            ChunkPtr = this->SpawnChunk(Chunk);
            this->ChunkMap.Add(Chunk, ChunkPtr);
        }
        else
        {
            ChunkPtr = *MapPtr;
        }

        ChunkPtr->SetChunkState(EChunkState::BlockedByHyperlane);

        continue;
    }

    return;
}

void UChunkGenerationSubsystem::SafeLoadVerticalChunk(const TArray<FChunkKey>& Chunks)
{
    for (const FChunkKey& Chunk : Chunks)
    {
        const TObjectPtr<ACommonChunk>* MapPtr = this->ChunkMap.Find(Chunk);
        ACommonChunk* ChunkPtr;
        if (MapPtr == nullptr)
        {
            ChunkPtr = this->SpawnChunk(Chunk);
            this->ChunkMap.Add(Chunk, ChunkPtr);
        }
        else
        {
            ChunkPtr = *MapPtr;
        }

        ChunkPtr->SetChunkState(EChunkState::Spawned);
        ChunkPtr->SetChunkState(EChunkState::Shaped);
        ChunkPtr->SetChunkState(EChunkState::SurfaceReplaced);
        ChunkPtr->SetChunkState(EChunkState::Active);

        continue;
    }

    return;
}

void UChunkGenerationSubsystem::DequeueNextVerticalChunkToKill(void)
{
    FChunkKey2 NewKillKey;
    if (this->PendingKillVerticalChunkQueue.Dequeue(NewKillKey) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    for (int32 Z = 0; Z < this->CopiedChunksAboveZero; ++Z)
    {
        this->ChunkMap.FindAndRemoveChecked(FChunkKey(NewKillKey.X, NewKillKey.Y, Z))->KillUncontrolled();
    }

    if (this->VerticalChunks.Remove(NewKillKey) != 1)
    {
        LOG_ERROR(LogChunkGeneration, "Something went wrong while removeing the vertical chunk [%d::%d].", NewKillKey.X, NewKillKey.Y)
    }

#if LOG_PERFORMANCE_CRITICAL_SECTIONS
    LOG_VERY_VERBOSE(LogChunkGeneration, "Removed vertical chunk [%d::%d].", NewKillKey.X, NewKillKey.Y)
#endif /* LOG_PERFORMANCE_CRITICAL_SECTIONS */

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
