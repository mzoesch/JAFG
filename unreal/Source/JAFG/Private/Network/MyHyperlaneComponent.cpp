// Copyright 2024 mzoesch. All rights reserved.

#include "Network/MyHyperlaneComponent.h"

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "TransmittableData.h"

UMyHyperlaneComponent::UMyHyperlaneComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

void UMyHyperlaneComponent::BeginPlay(void)
{
    Super::BeginPlay();

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

bool UMyHyperlaneComponent::MyAskServerForVoxelData_ServerRPC_Validate(const FChunkKey& ChunkKey)
{
    return true;
}

void UMyHyperlaneComponent::MyAskServerForVoxelData_ServerRPC_Implementation(const FChunkKey& ChunkKey)
{
    FClientChunk ClientChunk;
    ClientChunk.ChunkKey = ChunkKey;
    ClientChunk.Callback = TFunction<void(voxel_t*)>( [this, ChunkKey] (voxel_t* VoxelData)
    {
        check( VoxelData )
        this->SendChunkInitializationDataToClient(ChunkKey, VoxelData);
        return;
    });

    this->ChunkGenerationSubsystem->AddClientChunk(ClientChunk);

    return;
}

void UMyHyperlaneComponent::OnChunkInitializationDataReceived(const TransmittableData::FChunkInitializationData& Data)
{
    if (UNetStatics::IsSafeClient(this) == false)
    {
        LOG_FATAL(LogTemp, "Called on a non-client instance. Disallowed.")
        return;
    }

    check( this->ChunkGenerationSubsystem )
    this->ChunkGenerationSubsystem->SetInitializationDataFromAuthority(Data.ChunkKey, Data.Voxels);

    return;
}
