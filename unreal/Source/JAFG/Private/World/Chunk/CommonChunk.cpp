// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/CommonChunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Network/NetworkStatics.h"
#include "System/MaterialSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Chunk/ChunkMeshData.h"
#include "World/Voxel/CommonVoxels.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = true;

    this->ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    // We can do this for far away chunks maybe?
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    return;
}

void ACommonChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /*
     * Can we optimise this?
     */

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(ACommonChunk, RawVoxels, SharedParams);

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeStandalone(this) || UNetworkStatics::IsSafeServer(this))
    {
        this->WorldGeneratorInfo = CastChecked<AWorldGeneratorInfo>(
            UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));
    }

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check(this->VoxelSubsystem)
    if (this->VoxelSubsystem == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ACommonChunk::BeginPlay: Could not get Voxel Subsystem."))
        return;
    }

    this->Initialize();

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

    this->bInitializedClientBeginPlay = true;

    /* Just a safety net. */
    this->ClearMesh();

    this->GenerateProceduralMesh();
    this->ApplyProceduralMesh();

    return;
}

void ACommonChunk::Initialize(void)
{
    this->RawVoxels.SetNum(
        /* cubic */ AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize,
                    false);
    this->JChunkPosition = this->GetActorLocation() * AWorldGeneratorInfo::UToJScale;
    this->ChunkKey = FIntVector(this->JChunkPosition / (AWorldGeneratorInfo::ChunkSize - 1));
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

    UE_LOG(LogTemp, Error, TEXT("ACommonChunk::GenerateVoxels: World Generation of type %s not implemented."),
           *WorldGeneratorInfo::LexToString(this->WorldGeneratorInfo->WorldGenerationType));

    return;
}

void ACommonChunk::OnRep_RawVoxels(void)
{
    if (this->bInitializedClientBeginPlay == false)
    {
        UE_LOG(LogTemp, Warning,
               TEXT(
                   "ACommonChunk::OnRep_RawVoxels: Discarding Raw Voxels replication post behavior as this chunk has not yet be marked as initialized."
               ))
        return;
    }

    this->ClearMesh();
    this->GenerateProceduralMesh();
    this->ApplyProceduralMesh();

    return;
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
