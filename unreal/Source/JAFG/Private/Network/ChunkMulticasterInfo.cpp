// Copyright 2024 mzoesch. All rights reserved.

#include "Network/ChunkMulticasterInfo.h"

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Chunk/CommonChunk.h"

AChunkMulticasterInfo::AChunkMulticasterInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates      = true;
    this->bNetLoadOnClient = true;
    this->bAlwaysRelevant  = true;

    return;
}

void AChunkMulticasterInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetStatics::IsSafeClient(this) == false)
    {
        return;
    }

    this->LocalChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->LocalChunkGenerationSubsystem )

    return;
}

void AChunkMulticasterInfo::BroadcastChunkModification_NetMulticastRPC_Implementation(const FChunkKey& ChunkKey,  const FVoxelKey& LocalVoxelKey, const voxel_t Voxel)
{
    /*
     * Network multicast remote-procedure-calls called from the server will also be executed on the server and not just
     * on the clients. But we already treated and validated the data on the server, so we can safely ignore this call.
     */
    if (UNetStatics::IsSafeClient(this) == false)
    {
        return;
    }

    ACommonChunk* ChangedTargetChunk = this->LocalChunkGenerationSubsystem->FindChunkByKey(ChunkKey);

    /* Target chunk is not in render distance. */
    if (ChangedTargetChunk == nullptr)
    {
        return;
    }

    LOG_VERY_VERBOSE(
        LogChunkManipulation,
        "Requested to modify single voxel at %s to %d in %s.",
        *LocalVoxelKey.ToString(), Voxel, *ChunkKey.ToString()
    )

    ChangedTargetChunk->ModifySingleVoxelOnClient(LocalVoxelKey, Voxel);

    return;
}
