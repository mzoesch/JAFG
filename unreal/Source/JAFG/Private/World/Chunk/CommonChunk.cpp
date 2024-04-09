// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/CommonChunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Network/HyperlaneTransmitterInfo.h"
#include "Network/NetworkStatics.h"
#include "System/MaterialSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Chunk/ChunkMeshData.h"
#include "World/Voxel/CommonVoxels.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;

    this->ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    // We can do this for far away chunks maybe?
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeServer(this))
    {
        this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));
        check (this->WorldGeneratorInfo )
    }
    else
    {
        if (Cast<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass())))
        {
            UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::BeginPlay: Found World Generator Info on a client. This is disallowed."))
            return;
        }
    }

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check(this->VoxelSubsystem)
    if (this->VoxelSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::BeginPlay: Could not get Voxel Subsystem."))
        return;
    }

    this->PreInitialize();

    if (UNetworkStatics::IsSafeServer(this))
    {
        this->GenerateVoxels();

        /* We here should of course only do convex meshing in the future. */
        this->GenerateProceduralMesh();
        this->ApplyProceduralMesh();

        return;
    }

    /*
     * Voxels are generated on the server and replicated to the client.
     * We do not have to care about that here.
     */

    return;
}

void ACommonChunk::PreInitialize(void)
{
    this->RawVoxels.SetNum(AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize, false);
    this->JChunkPosition = this->GetActorLocation() * AWorldGeneratorInfo::UToJScale;
    this->ChunkKey = FIntVector(this->JChunkPosition / (AWorldGeneratorInfo::ChunkSize - 1));
}

void ACommonChunk::ApplyProceduralMesh(void) const
{
    const UMaterialSubsystem* MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();

    if (MaterialSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::ApplyProceduralMesh: Could not get Material Subsystem."))
        return;
    }

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        if (i == ETextureGroup::Opaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::Opaque, MaterialSubsystem->MDynamicOpaque);
        }
        else if (i == ETextureGroup::FullBlendOpaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FullBlendOpaque,
                                                       MaterialSubsystem->MDynamicFullBlendOpaque);
        }
        else if (i == ETextureGroup::FloraBlendOpaque)
        {
            this->ProceduralMeshComponent->SetMaterial(ETextureGroup::FloraBlendOpaque,
                                                       MaterialSubsystem->MDynamicFloraBlendOpaque);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ACommonChunk::ApplyProceduralMesh: Texture group %d not implemented."), i)
            this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicOpaque);
        }

        this->ProceduralMeshComponent->CreateMeshSection(
            i,
            this->MeshData[i].Vertices,
            this->MeshData[i].Triangles,
            this->MeshData[i].Normals,
            this->MeshData[i].UV0,
            this->MeshData[i].Colors,
            this->MeshData[i].Tangents,
            true
        );
    }
}

void ACommonChunk::ClearMesh()
{
    /* TODO We might want to use arr.empty? */

    for (int i = 0; i < this->MeshData.Num(); ++i)
    {
        this->MeshData[i].Clear();
    }

    for (int i = 0; i < this->VertexCounts.Num(); ++i)
    {
        this->VertexCounts[i] = 0;
    }

    return;
}

void ACommonChunk::GenerateVoxels(void)
{
    switch (this->WorldGeneratorInfo->WorldGenerationType)
    {
    case EWorldGenerationType::Default:
    {
    }
    case EWorldGenerationType::SuperFlat:
    {
        this->GenerateSuperFlatWorld();
        return;
    }
    default:
    {
    }
    }

    UE_LOG(LogTemp, Error, TEXT("ACommonChunk::GenerateVoxels: World Generation of type %s not implemented."), *WorldGeneratorInfo::LexToString(this->WorldGeneratorInfo->WorldGenerationType));

    return;
}

void ACommonChunk::GenerateSuperFlatWorld()
{
    /*
     * We of course should make this modular with some params that maybe even the user can provide.
     */

    constexpr int StoneVoxel{2};
    constexpr int DirtVoxel{3};
    constexpr int GrassVoxel{4};

    for (int X = 0; X < AWorldGeneratorInfo::ChunkSize; ++X)
    {
        for (int Y = 0; Y < AWorldGeneratorInfo::ChunkSize; ++Y)
        {
            for (int Z = 0; Z < AWorldGeneratorInfo::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;

                if (WorldZ < 10)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = StoneVoxel;
                    continue;
                }

                if (WorldZ < 11)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = DirtVoxel;
                    continue;
                }

                if (WorldZ == 11)
                {
                    this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = GrassVoxel;
                    continue;
                }

                this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = ECommonVoxels::Air;

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void ACommonChunk::SendInitializationDataToClient(AWorldPlayerController* Target) const
{
    check ( Target )

    LOG_VERBOSE(LogChunkValidation, "Sending packaged initialization data to client for chunk %s.", *this->ChunkKey.ToString())

    AHyperlaneTransmitterInfo* Transmitter = Cast<AHyperlaneTransmitterInfo>(UGameplayStatics::GetActorOfClass(this, AHyperlaneTransmitterInfo::StaticClass()));
    check( Transmitter )

    TransmittableData::FChunkInitializationData Data = TransmittableData::FChunkInitializationData(this->ChunkKey, this->RawVoxels);
    Transmitter->SendChunkInitializationData(Target, Data);

    return;
}

void ACommonChunk::InitializeWithAuthorityData(const TArray<int32>& InRawVoxels)
{
    this->RawVoxels = InRawVoxels;

    UE_LOG(LogTemp, Warning, TEXT("ACommonChunk::InitializeWithAuthorityData: Generating chunk %s with authority data."), *this->ChunkKey.ToString())

    this->ClearMesh();
    this->GenerateProceduralMesh();
    this->ApplyProceduralMesh();

    return;
}

ACommonChunk* ACommonChunk::GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only the server can get target chunks.")
        return nullptr;
    }

    FIntVector TransformedChunkKey   = this->ChunkKey;
    OutTransformedLocalVoxelPosition = LocalVoxelPosition;

    if (LocalVoxelPosition.X >= AWorldGeneratorInfo::ChunkSize)
    {
        TransformedChunkKey.X++;
        OutTransformedLocalVoxelPosition.X -= AWorldGeneratorInfo::ChunkSize;
    }

    else if (LocalVoxelPosition.X < 0)
    {
        TransformedChunkKey.X--;
        OutTransformedLocalVoxelPosition.X += AWorldGeneratorInfo::ChunkSize;
    }

    if (LocalVoxelPosition.Y >= AWorldGeneratorInfo::ChunkSize)
    {
        TransformedChunkKey.Y++;
        OutTransformedLocalVoxelPosition.Y -= AWorldGeneratorInfo::ChunkSize;
    }

    else if (LocalVoxelPosition.Y < 0)
    {
        TransformedChunkKey.Y--;
        OutTransformedLocalVoxelPosition.Y += AWorldGeneratorInfo::ChunkSize;
    }

    if (LocalVoxelPosition.Z >= AWorldGeneratorInfo::ChunkSize)
    {
        TransformedChunkKey.Z++;
        OutTransformedLocalVoxelPosition.Z -= AWorldGeneratorInfo::ChunkSize;
    }

    else if (LocalVoxelPosition.Z < 0)
    {
        TransformedChunkKey.Z--;
        OutTransformedLocalVoxelPosition.Z += AWorldGeneratorInfo::ChunkSize;
    }

    if (OutTransformedLocalVoxelPosition == LocalVoxelPosition)
    {
        return this;
    }

    LOG_VERBOSE(LogChunkManipulation, "Transformed local voxel position from %s to %s. Key change from %s to %s.",
        *LocalVoxelPosition.ToString(), *OutTransformedLocalVoxelPosition.ToString(), *this->ChunkKey.ToString(), *TransformedChunkKey.ToString())

    check( this->WorldGeneratorInfo )
    ACommonChunk* TargetChunk = this->WorldGeneratorInfo->GetChunkBy(TransformedChunkKey);

    if (TargetChunk == nullptr)
    {
        return nullptr;
    }

    const FIntVector Temp = OutTransformedLocalVoxelPosition;
    return TargetChunk->GetTargetChunk(Temp, OutTransformedLocalVoxelPosition);
}

void ACommonChunk::ModifySingleVoxel(const FIntVector& LocalVoxelPosition, const int NewVoxel)
{
    LOG_VERY_VERBOSE(LogChunkManipulation, "Requested to modify single voxel at %s to %d int %s.",
        *LocalVoxelPosition.ToString(), NewVoxel, *this->GetName())

    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogChunkManipulation, "Only the server can modify voxels.")
        return;
    }

    FIntVector TransformedLocalVoxelPosition = LocalVoxelPosition;
    ACommonChunk* TargetChunk = this->GetTargetChunk(LocalVoxelPosition, TransformedLocalVoxelPosition);

    if (TargetChunk == nullptr)
    {
        LOG_ERROR(LogChunkManipulation, "Could not get target chunk for local voxel position %s (pivot: %s). Modfied to %s.",
            *LocalVoxelPosition.ToString(), *this->GetName(), *TransformedLocalVoxelPosition.ToString())
        return;
    }

    if (TargetChunk->GetRawVoxelData(TransformedLocalVoxelPosition) == NewVoxel)
    {
        LOG_WARNING(LogChunkManipulation, "Voxel at %s already has the value %d. No need to modify.",
            *TransformedLocalVoxelPosition.ToString(), NewVoxel)
        return;
    }

    TargetChunk->ModifyRawVoxelData(TransformedLocalVoxelPosition, NewVoxel);

    /*
     * In the future we might want to look at this one more time
     * and try to only update a small percentage of the mesh.
     */
    /*
     * Here we of course should only do convex meshing.
     */
    TargetChunk->ClearMesh();
    TargetChunk->GenerateProceduralMesh();
    TargetChunk->ApplyProceduralMesh();

    return;
}

FIntVector ACommonChunk::WorldToLocalVoxelLocation(const FVector& WorldLocation)
{
    FIntVector WorldToChunkPosition;

    const int Factor = AWorldGeneratorInfo::ChunkSize * 100;
    const auto IntPosition = FIntVector(WorldLocation);

    if (IntPosition.X < 0) WorldToChunkPosition.X = (int) (WorldLocation.X / Factor) - 1;
    else WorldToChunkPosition.X = (int) (WorldLocation.X / Factor);

    if (IntPosition.Y < 0) WorldToChunkPosition.Y = (int) (WorldLocation.Y / Factor) - 1;
    else WorldToChunkPosition.Y = (int) (WorldLocation.Y / Factor);

    if (IntPosition.Z < 0) WorldToChunkPosition.Z = (int) (WorldLocation.Z / Factor) - 1;
    else WorldToChunkPosition.Z = (int) (WorldLocation.Z / Factor);

    /* WorldToBlockPosition */
    FIntVector WorldToBlockPosition = FIntVector(WorldLocation) / 100 - WorldToChunkPosition * AWorldGeneratorInfo::ChunkSize;

    /* Negative Normalization */
    if (WorldToChunkPosition.X < 0) WorldToBlockPosition.X--;
    if (WorldToChunkPosition.Y < 0) WorldToBlockPosition.Y--;
    if (WorldToChunkPosition.Z < 0) WorldToBlockPosition.Z--;

    return WorldToBlockPosition;
}
