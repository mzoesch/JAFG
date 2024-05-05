// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/CommonChunk.h"

#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = false;

    this->ProceduralMeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    /* We can do this for far away chunks, maybe? */
    /* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
    this->ProceduralMeshComponent->SetCastShadow(true);

    return;
}

void ACommonChunk::BeginPlay(void)
{
    Super::BeginPlay();

    this->PreInitialize();

    this->GenerateVoxels();

    this->RegenerateProceduralMesh();

    return;
}

void ACommonChunk::PreInitialize(void)
{
    this->RawVoxelData.SetNum(WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize, false);
    this->JChunkPosition = this->GetActorLocation() * WorldStatics::UToJScale;
    this->ChunkKey       = FChunkKey(this->JChunkPosition / (WorldStatics::ChunkSize - 1));

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( this->VoxelSubsystem )

    return;
}

#pragma region Procedural Mesh

void ACommonChunk::ApplyProceduralMesh(void)
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

    return;
}

#pragma endregion Procedural Mesh

#pragma region Chunk World Generation

void ACommonChunk::GenerateVoxels(void)
{
    this->GenerateSuperFlatWorld();
}

void ACommonChunk::GenerateSuperFlatWorld(void)
{
    /*
     * We should make this modular with some params that maybe even the user can provide.
     */

    constexpr int StoneVoxel { 2 };
    constexpr int DirtVoxel  { 3 };
    constexpr int GrassVoxel { 4 };

    for (int X = 0; X < WorldStatics::ChunkSize; ++X)
    {
        for (int Y = 0; Y < WorldStatics::ChunkSize; ++Y)
        {
            for (int Z = 0; Z < WorldStatics::ChunkSize; ++Z)
            {
                const float WorldZ = this->JChunkPosition.Z + Z;

                if (WorldZ < 10)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), StoneVoxel);
                    continue;
                }

                if (WorldZ < 11)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), DirtVoxel);
                    continue;
                }

                if (WorldZ == 11)
                {
                    this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), GrassVoxel);
                    continue;
                }

                this->ModifyRawVoxelData(FVoxelKey(X, Y, Z), ECommonVoxels::Air);

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

#pragma endregion Chunk World Generation
