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

void AChunkMulticasterInfo::MulticastChunkModification_Implementation(const FIntVector& ChunkKey, const FIntVector& LocalVoxel, const int32 VoxelValue)
{
    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        return;
    }

    LOG_WARNING(LogChunkMisc, "Updating %s at %s with %d.", *ChunkKey.ToString(), *LocalVoxel.ToString(), VoxelValue)

    check( this->LocalPlayerChunkGeneratorSubsystem )

    ACommonChunk* Chunk = this->LocalPlayerChunkGeneratorSubsystem->LoadedChunks.Contains(ChunkKey)
        ? this->LocalPlayerChunkGeneratorSubsystem->LoadedChunks[ChunkKey] : nullptr;

    if (Chunk == nullptr)
    {
        LOG_WARNING(LogChunkMisc, "Chunk %s not found.", *ChunkKey.ToString())
        return;
    }

    Chunk->ModifySingleVoxelOnClient(LocalVoxel, VoxelValue);

    return;
}
