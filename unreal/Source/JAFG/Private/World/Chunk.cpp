// © 2023 mzoesch. All rights reserved.

#include "World/Chunk.h"

#include "Lib/FastNoiseLite.h"
#include "ProceduralMeshComponent.h"
#include "World/ChunkWorld.h"

AChunk::AChunk()
{
    // Set this actor to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    this->Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
    this->Mesh->SetCastShadow(true);
    this->SetRootComponent(this->Mesh);

    this->Noise = new FastNoiseLite();
    
    return;
}

void AChunk::BeginPlay()
{
    Super::BeginPlay();

    // TODO
    //      This is not necessary anymore.
    //      We should use the same noise for all objects and store this in the ChunkWorld.

    this->Noise->SetFrequency(AChunkWorld::DevFrequency);
    this->Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    this->Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

    this->Setup();
    this->InitiateVoxels();
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}

void AChunk::ApplyMesh() const
{
    this->Mesh->SetMaterial(0, this->Material);
    this->Mesh->CreateMeshSection(
        0,
        this->MeshData.Vertices,
        this->MeshData.Triangles,
        this->MeshData.Normals,
        this->MeshData.UV0,
        this->MeshData.Colors,
        this->MeshData.Tangents,
        true
    );

    return;
}

void AChunk::ClearMesh()
{
    this->VertexCount = 0;
    this->MeshData.Clear();
    return;
}

void AChunk::ModifyVoxel(const FIntVector& LocalVoxelPosition, const EVoxel Voxel)
{
    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        UE_LOG(LogTemp, Warning, TEXT("Voxel out of bounds! Not implemented yet! %s"), *LocalVoxelPosition.ToString());
        return;
    }
    
    this->ModifyVoxelData(LocalVoxelPosition, Voxel);
    this->ClearMesh(); /* TODO Do we really have to regenerate the whole mesh? */
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}
