// Copyright 2024 mzoesch. All rights reserved.

#include "Network/ChunkMulticasterInfo.h"

#include "Network/NetworkStatics.h"
#include "World/Chunk/CommonChunk.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"

AChunkMulticasterInfo::AChunkMulticasterInfo(const FObjectInitializer& ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = true;
    this->bNetLoadOnClient = true;
    this->bAlwaysRelevant = true;

    return;
}

void AChunkMulticasterInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeDedicatedServer(this))
    {
        return;
    }

    check( GEngine )
    check( this->GetWorld() )
    check( GEngine->GetFirstGamePlayer(this->GetWorld()) )
    this->LocalPlayerChunkGeneratorSubsystem = GEngine->GetFirstGamePlayer(this->GetWorld())->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
    check( this->LocalPlayerChunkGeneratorSubsystem )

    return;
}

void AChunkMulticasterInfo::MulticastChunkModification_Implementation(const FIntVector& ChunkKey, const FIntVector& LocalVoxelLocation, const int32 Voxel)
{
    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        return;
    }

    check( this->LocalPlayerChunkGeneratorSubsystem )

    ACommonChunk* Chunk = this->LocalPlayerChunkGeneratorSubsystem->LoadedChunks.Contains(ChunkKey)
        ? this->LocalPlayerChunkGeneratorSubsystem->LoadedChunks[ChunkKey] : nullptr;

    if (Chunk == nullptr)
    {
        LOG_WARNING(LogChunkMisc, "Chunk %s not found.", *ChunkKey.ToString())
        return;
    }

    LOG_VERY_VERBOSE(LogChunkManipulation, "Requested to modify single voxel at %s to %d in %s.",
        *LocalVoxelLocation.ToString(), Voxel, *ChunkKey.ToString())

    Chunk->ModifySingleVoxelOnClient(LocalVoxelLocation, Voxel);

    return;
}
