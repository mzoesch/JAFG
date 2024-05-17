// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

#include "WorldCore/ChunkWorldSettings.h"
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

        if (this->bInClientMode && ChunksGenerated >= 2)
        {
            break;
        }

    }

    if (this->bInClientMode)
    {
        return;
    }

    // Check for client needs
    //////////////////////////////////////////////////////////////////////////
    int ChunkAnswered = 0;
    while (
           ChunkAnswered < 10
        && this->ClientQueue.IsEmpty() == false
    )
    {
        this->DequeueNextClientChunk();
        ChunkAnswered++;
    }

    return;
}

void UChunkGenerationSubsystem::AddVerticalChunkToPendingKillQueue(const FChunkKey2& ChunkKey)
{
    if (this->PendingKillVerticalChunkQueue.Contains(ChunkKey))
    {
        return;
    }

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

    if (this->VerticalChunks.Contains(NewActiveKey) == false)
    {
        this->VerticalChunks.Add(NewActiveKey);
    }

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
#if WITH_EDITOR
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogChunkGeneration, "Called on a client instance. Dissallowed.")
        return;
    }
#endif /* WITH_EDITOR */

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

void UChunkGenerationSubsystem::SafeLoadVerticalChunk(
    const TArray<FChunkKey>& Chunks,
    const bool bGenerateMesh /* = true */,
    const EChunkPersistency::Type Persistency /* = EChunkPersistency::Persistent */,
    const float TimeToLive /* = 0.0f */
)
{
    for (const FChunkKey& Chunk : Chunks)
    {
        const TObjectPtr<ACommonChunk>* MapPtr = this->ChunkMap.Find(Chunk);
        ACommonChunk* ChunkPtr;
        if (MapPtr == nullptr)
        {
            ChunkPtr = this->SpawnChunk(Chunk);
            this->ChunkMap.Add(Chunk, ChunkPtr);
            ChunkPtr->SetChunkPersistency(Persistency, TimeToLive);
        }
        else
        {
            ChunkPtr = *MapPtr;
            /*
             * If the chunk is in a persistent state. We may never set it back to a temporary state.
             */
            if (ChunkPtr->GetChunkPersistency() == EChunkPersistency::Temporary)
            {
                ChunkPtr->SetChunkPersistency(Persistency, TimeToLive);
            }
        }

        if (ChunkPtr->GetChunkState() < EChunkState::Spawned)         { ChunkPtr->SetChunkState(EChunkState::Spawned);         }
        if (ChunkPtr->GetChunkState() < EChunkState::Shaped)          { ChunkPtr->SetChunkState(EChunkState::Shaped);          }
        if (ChunkPtr->GetChunkState() < EChunkState::SurfaceReplaced) { ChunkPtr->SetChunkState(EChunkState::SurfaceReplaced); }

        if (bGenerateMesh && ChunkPtr->GetChunkState() < EChunkState::Active)
        {
            ChunkPtr->SetChunkState(EChunkState::Active);
        }

        continue;
    }

    return;
}

void UChunkGenerationSubsystem::DequeueNextClientChunk(void)
{
#if WITH_EDITOR
    if (UNetStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkGeneration, "Called on a client instance. Dissallowed.")
        return;
    }
#endif /* WITH_EDITOR */

    FClientChunk Entry;
    if (this->ClientQueue.Dequeue(Entry) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    TArray<FChunkKey> AllVerticalChunksOfRequest3DChunks = TArray<FChunkKey>();
    AllVerticalChunksOfRequest3DChunks.Reserve(this->CopiedChunksAboveZero);

    for (int32 Z = 0; Z < this->CopiedChunksAboveZero; ++Z)
    {
        AllVerticalChunksOfRequest3DChunks.Add(FChunkKey(Entry.ChunkKey.X, Entry.ChunkKey.Y, Z));
    }

    this->SafeLoadVerticalChunk(
        AllVerticalChunksOfRequest3DChunks,
        false,
        EChunkPersistency::Temporary,
        10.0f
    );

    this->ChunkMap[Entry.ChunkKey]->SendDataToClient(Entry.Callback);

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
        this->LocalChunkWorldSettings ?
            this->LocalChunkWorldSettings->LocalChunkType == EChunkType::Greedy
                ? AGreedyChunk::StaticClass()
                : ACommonChunk::StaticClass()
            : AGreedyChunk::StaticClass()
        ,

        TargetedChunkTransform
    );

    return Chunk;
}
