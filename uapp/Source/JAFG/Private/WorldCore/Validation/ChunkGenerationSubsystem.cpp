// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkGenerationSubsystem.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "RegisteredWorldNames.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ChunkMulticasterInfo.h"
#include "WorldCore/Chunk/ChunkArena.h"
#include "WorldCore/Chunk/GreedyChunk.h"
#include "WorldCore/Chunk/ChunkStates.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"

UChunkGenerationSubsystem::UChunkGenerationSubsystem(void) : Super(), CommonChunkParams()
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

    this->ChunkArena = InWorld.GetSubsystem<UChunkArena>();

    if (UNetStatics::IsSafeDedicatedServer(&InWorld) == false)
    {
        this->LocalChunkWorldSettings = InWorld.GetSubsystem<ULocalChunkWorldSettings>();
        jcheck( this->LocalChunkWorldSettings )
    }

    if (UNetStatics::IsSafeServer(&InWorld))
    {
        this->ServerChunkWorldSettings = InWorld.GetSubsystem<UServerChunkWorldSettings>();
        jcheck( this->ServerChunkWorldSettings )

        this->bHasReceivedReplicatedServerChunkWorldSettings = true;
        this->CopiedChunksAboveZero = this->ServerChunkWorldSettings->ChunksAboveZero;
    }

    if (UNetStatics::IsSafeClient(&InWorld))
    {
        this->bInClientMode = true;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        this->bHasReceivedReplicatedServerChunkWorldSettings = true;

        check( this->ServerChunkWorldSettings )
        this->CopiedChunksAboveZero = this->ServerChunkWorldSettings->ChunksAboveZero;
    }
#endif /* WITH_EDITOR */

    this->CommonChunkParams.VoxelSubsystem           = InWorld.GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    this->CommonChunkParams.ChunkGenerationSubsystem = this;
    this->CommonChunkParams.ChunkMulticasterInfo     = Cast<AChunkMulticasterInfo>(UGameplayStatics::GetActorOfClass(&InWorld, AChunkMulticasterInfo::StaticClass()));
    this->CommonChunkParams.MaterialSubsystem        = InWorld.GetGameInstance()->GetSubsystem<UMaterialSubsystem>();
    this->CommonChunkParams.ServerChunkWorldSettings = this->ServerChunkWorldSettings;

    jcheck( this->CommonChunkParams.VoxelSubsystem )
    jcheck( this->CommonChunkParams.ChunkGenerationSubsystem )
    jcheck( this->CommonChunkParams.ChunkMulticasterInfo )
    jcheck( this->CommonChunkParams.MaterialSubsystem )

    return;
}

void UChunkGenerationSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    if (this->bHasReceivedReplicatedServerChunkWorldSettings == false)
    {
        if (
            const UServerWorldSettingsReplicationComponent* Component =
            GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetComponentByClass<UServerWorldSettingsReplicationComponent>();
            Component == nullptr || Component->HasReplicatedSettings() == false
        )
        {
            return;
        }

        jcheck( this->LocalChunkWorldSettings )

        this->CopiedChunksAboveZero = this->LocalChunkWorldSettings->GetReplicatedChunksAboveZero();
        jcheck( this->CopiedChunksAboveZero > 0 )

        this->bHasReceivedReplicatedServerChunkWorldSettings = true;

        LOG_DISPLAY(
            LogChunkGeneration,
            "Received server world settings replication. Chunk generation is now ready. Waiting for validation subsystem."
        )
    }

    checkSlow( this->CopiedChunksAboveZero > 0 )

    // Unloading
    //////////////////////////////////////////////////////////////////////////
    while (this->PendingKillVerticalChunkQueue.IsEmpty() == false)
    {
        this->DequeueNextVerticalChunkToKill();
    }

    // Reserving and Freeing
    //////////////////////////////////////////////////////////////////////////
    if (this->ChunkArena->GetFreeChunkCount() < this->MaxVerticalChunksToGeneratePerTick * this->CopiedChunksAboveZero * 1.5f)
    {
        this->ChunkArena->ReserveGladiators(
            this->MaxVerticalChunksToGeneratePerTick * this->CopiedChunksAboveZero * 2
        );
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
        ++ChunksGenerated;

        /* Relaxes the hyperlane. */
        if (this->bInClientMode && ChunksGenerated >= 2)
        {
            break;
        }
    }

    if (this->bInClientMode == false)
    {
        // Check for client needs
        //////////////////////////////////////////////////////////////////////////
        int ChunksAnswered = 0;
        while (
               ChunksAnswered < this->MaxVerticalChunksToAnswerPerTick
            && this->ClientQueue.IsEmpty() == false
        )
        {
            this->DequeueNextClientChunk();
            ++ChunksAnswered;
        }
    }

    if (++this->CurrentPureInterval > this->PurgeInterval)
    {
        this->CurrentPureInterval = 0;

        // Check for the end of life in temporary chunks
        //////////////////////////////////////////////////////////////////////////
        TArray<ACommonChunk*> ChunksToKill = TArray<ACommonChunk*>();
        for (const TTuple<FChunkKey, TObjectPtr<ACommonChunk>>& Pair : this->ChunkMap)
        {
            if (Pair.Value->ShouldBeKilled())
            {
                ChunksToKill.Add(Pair.Value);
            }
        }
        for (ACommonChunk* Chunk : ChunksToKill)
        {
            check( Chunk->GetChunkState() != EChunkState::Freed )

            LOG_VERY_VERBOSE(LogChunkMisc, " Killing chunk %s due to persistency.", *Chunk->GetChunkKeyOnTheFly(true).ToString())
            Chunk->SetChunkState(EChunkState::Kill);
        }
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

void UChunkGenerationSubsystem::FreeMe(ACommonChunk& Chunk)
{
    this->ChunkArena->Free(Chunk);

    const FChunkKey Key = Chunk.GetChunkKeyOnTheFly(true);
    this->ChunkMap.Remove(Key);
    this->VerticalChunks.Remove(FChunkKey2(Key.X, Key.Y));

    return;
}

bool UChunkGenerationSubsystem::FindAppropriateLocationForCharacterSpawn(const FVector2D& InApproximateLocation, FVector& OutLocation) const
{
    if (this->GetWorld()->GetName() == RegisteredWorlds::Dev)
    {
        OutLocation = FVector(0.0f, 0.0f, 300.0f);
        return true;
    }

    const FChunkKey2    TargetVKey = WorldStatics::WorldToVerticalChunkKey(InApproximateLocation);
    const FJCoordinate2 TargetJKey = WorldStatics::WorldToVerticalJCoordinate(InApproximateLocation);

    if (this->HasPersistentVerticalChunk(TargetVKey) == false)
    {
        return false;
    }

    TArray<FChunkKey> PossibleSpawnChunks = TArray<FChunkKey>();
    this->GetAllChunksFromVerticalChunkReversed(TargetVKey, PossibleSpawnChunks);
    check( PossibleSpawnChunks.Num() > 0 )

    for (FChunkKey CurrentTarget : PossibleSpawnChunks)
    {
        const ACommonChunk* Chunk = this->FindChunkByKey(CurrentTarget);
        if (Chunk == nullptr)
        {
            continue;
        }

        for (int32 Z = WorldStatics::ChunkSize - 1; Z >= 0; --Z)
        {
            if (Chunk->GetLocalVoxelOnly(FJCoordinate(TargetJKey.X, TargetJKey.Y, Z)) == ECommonVoxels::Air)
            {
                continue;
            }

            OutLocation = WorldStatics::JCoordinateToWorldLocation(FJCoordinate(
                TargetJKey.X,
                TargetJKey.Y,
                /*
                 * +1 To get the above air voxel where we want to spawn.
                 * +1 Extra voxel buffer to avoid unwanted collisions inside the procedural mesh.
                 */
                CurrentTarget.Z * WorldStatics::ChunkSize + (Z + 2)
            ));

            return true;
        }

        continue;
    }

    return false;
}

bool UChunkGenerationSubsystem::FindAppropriateLocationForCharacterSpawn(const FVector& InApproximateLocation, FVector& OutLocation) const
{
    return this->FindAppropriateLocationForCharacterSpawn(FVector2D(InApproximateLocation.X, InApproximateLocation.Y), OutLocation);
}

void UChunkGenerationSubsystem::DequeueNextVerticalChunk(void)
{
    FChunkKey2 NewActiveKey;
    if (this->VerticalChunkQueue.Dequeue(NewActiveKey) == false)
    {
        LOG_WARNING(LogChunkGeneration, "Called but the queue was empty.")
        return;
    }

    TArray<FChunkKey> NewChunks = TArray<FChunkKey>();
    this->GetAllChunksFromVerticalChunk(NewActiveKey, NewChunks);

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
        LOG_FATAL(LogChunkGeneration, "Called on a non client instance. Dissallowed.")
        return;
    }
#endif /* WITH_EDITOR */

    struct FEntry { FChunkKey ChunkKey; ACommonChunk* ChunkPtr; };
    TArray<FEntry> Iterator;
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

        Iterator.Emplace(FEntry{ Chunk, ChunkPtr });

        continue;
    }

    this->PrepareWorldForChunkTransit_Spawned(FChunkKey2(Chunks[0].X, Chunks[0].Y));
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    {
        if (ChunkPtr->GetChunkState() < EChunkState::Spawned)
        {
            ChunkPtr->SetChunkState(EChunkState::BlockedByHyperlane);
        }
    }

    return;
}

void UChunkGenerationSubsystem::SafeLoadVerticalChunk(
    const TArray<FChunkKey>& Chunks,
    const bool bGenerateMesh /* = true */,
    const EChunkPersistency::Type Persistency /* = EChunkPersistency::Persistent */,
    const float TimeToLive /* = 0.0f */,
    const EChunkState::Type TargetState /* = EChunkState::Active */
)
{
    /* We can only generate chunks between those states. The other are special. */
    check( EChunkState::Freed < TargetState && TargetState < EChunkState::Special )

    struct FEntry { FChunkKey ChunkKey; ACommonChunk* ChunkPtr; };
    TArray<FEntry> Iterator;
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
            /* If the chunk is in a persistent state. We may never set it back to a temporary state. */
            if (ChunkPtr->GetChunkPersistency() == EChunkPersistency::Temporary)
            {
                ChunkPtr->SetChunkPersistency(Persistency, TimeToLive);
            }
        }

        Iterator.Emplace(FEntry{ Chunk, ChunkPtr });

        continue;
    }

    if (TargetState < EChunkState::Spawned) { return; }
    if (Iterator[0].ChunkPtr->GetChunkState() < EChunkState::Spawned)
    { this->PrepareWorldForChunkTransit_Spawned(FChunkKey2(Chunks[0].X, Chunks[0].Y)); }
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    { if (ChunkPtr->GetChunkState() < EChunkState::Spawned) { ChunkPtr->SetChunkState(EChunkState::Spawned); } }

    if (TargetState < EChunkState::Shaped) { return; }
    if (Iterator[0].ChunkPtr->GetChunkState() < EChunkState::Shaped)
    { this->PrepareWorldForChunkTransit_Shaped(FChunkKey2(Chunks[0].X, Chunks[0].Y)); }
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    { if (ChunkPtr->GetChunkState() < EChunkState::Shaped) { ChunkPtr->SetChunkState(EChunkState::Shaped); } }

    if (TargetState < EChunkState::SurfaceReplaced) { return; }
    if (Iterator[0].ChunkPtr->GetChunkState() < EChunkState::SurfaceReplaced)
    { this->PrepareWorldForChunkTransit_SurfaceReplaced(FChunkKey2(Chunks[0].X, Chunks[0].Y)); }
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    { if (ChunkPtr->GetChunkState() < EChunkState::SurfaceReplaced) { ChunkPtr->SetChunkState(EChunkState::SurfaceReplaced); } }

    if (TargetState < EChunkState::ShapedCaves) { return; }
    if (Iterator[0].ChunkPtr->GetChunkState() < EChunkState::ShapedCaves)
    { this->PrepareWorldForChunkTransit_ShapedCaves(FChunkKey2(Chunks[0].X, Chunks[0].Y)); }
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    { if (ChunkPtr->GetChunkState() < EChunkState::ShapedCaves) { ChunkPtr->SetChunkState(EChunkState::ShapedCaves); } }

    if (TargetState < EChunkState::Active) { return; }
    for (const auto& [ChunkKey, ChunkPtr] : Iterator)
    {
        if (bGenerateMesh && ChunkPtr->GetChunkState() < EChunkState::Active)
        {
            ChunkPtr->SetChunkState(EChunkState::Active);
        }
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
        this->ChunkArena->Free(*this->ChunkMap.FindAndRemoveChecked(FChunkKey(NewKillKey.X, NewKillKey.Y, Z)));
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

ACommonChunk* UChunkGenerationSubsystem::SpawnChunk(const FChunkKey& ChunkKey)
{
    IChunkArenaGladiator* Gladiator;
    this->ChunkArena->Alloc(ChunkKey, Gladiator);
    Gladiator->AsChunk()->ChunkParams = &this->CommonChunkParams;
    return Gladiator->AsChunk();
}

void UChunkGenerationSubsystem::PrepareWorldForChunkTransit_Spawned(const FChunkKey2& Chunk)
{
    for (const FChunkKey2& Neighbor : WorldStatics::GetNeighboringChunks(Chunk))
    {
        this->SafeLoadVerticalChunk(
            this->GetAllChunksFromVerticalChunk(Neighbor),
            false,
            EChunkPersistency::Temporary,
            40.0f,
            EChunkState::PreSpawned
        );
    }

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UChunkGenerationSubsystem::PrepareWorldForChunkTransit_Shaped(const FChunkKey2& Chunk)
{
    /* Shaping currently does not require any additional states more than spawned. */
}

void UChunkGenerationSubsystem::PrepareWorldForChunkTransit_SurfaceReplaced(const FChunkKey2& Chunk)
{
    for (const FChunkKey2& Neighbor : WorldStatics::GetNeighboringChunks(Chunk))
    {
        this->SafeLoadVerticalChunk(
            this->GetAllChunksFromVerticalChunk(Neighbor),
            false,
            EChunkPersistency::Temporary,
            40.0f,
            EChunkState::Spawned
        );
    }

    return;
}

void UChunkGenerationSubsystem::PrepareWorldForChunkTransit_ShapedCaves(const FChunkKey2& Chunk)
{
}

void UChunkGenerationSubsystem::GetAllChunksFromVerticalChunk(const FChunkKey2& ChunkKey, TArray<FChunkKey>& Out) const
{
    check( Out.IsEmpty() )

    Out.Reserve(this->CopiedChunksAboveZero);
    for (int32 Z = 0; Z < this->CopiedChunksAboveZero; ++Z)
    {
        Out.Emplace(FChunkKey(ChunkKey.X, ChunkKey.Y, Z));
    }

    return;
}

TArray<FChunkKey> UChunkGenerationSubsystem::GetAllChunksFromVerticalChunk(const FChunkKey2& ChunkKey) const
{
    TArray<FChunkKey> Out;
    this->GetAllChunksFromVerticalChunk(ChunkKey, Out);
    return Out;
}

void UChunkGenerationSubsystem::GetAllChunksFromVerticalChunkReversed(const FChunkKey2& ChunkKey, TArray<FChunkKey>& Out) const
{
    check( Out.IsEmpty() )

    Out.Reserve(this->CopiedChunksAboveZero);
    for (int32 Z = this->CopiedChunksAboveZero - 1; Z >= 0; --Z)
    {
        Out.Emplace(FChunkKey(ChunkKey.X, ChunkKey.Y, Z));
    }

    return;
}
