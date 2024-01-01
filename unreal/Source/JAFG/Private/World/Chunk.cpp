// © 2023 mzoesch. All rights reserved.

#include "World/Chunk.h"

#include "ProceduralMeshComponent.h"

#include "Lib/FastNoiseLite.h"
#include "Core/GI_Master.h"
#include "World/ChunkWorld.h"
#include "World/WorldVoxel.h"

AChunk::AChunk()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
    this->ProcMesh->SetCastShadow(true);
    this->SetRootComponent(this->ProcMesh);

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

    this->PreSetup();
    this->Setup();
    this->InitiateVoxels();
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}

void AChunk::PreSetup()
{
    this->Voxels.SetNum(AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE, false);
    return;
}

void AChunk::ApplyMesh() const
{
    this->ProcMesh->SetMaterial(0, CastChecked<UGI_Master>(this->GetGameInstance())->DevMaterial);
    this->ProcMesh->SetMaterial(1, CastChecked<UGI_Master>(this->GetGameInstance())->TranslucentMaterial);

    this->ProcMesh->CreateMeshSection(
        0,
        this->MeshData.Vertices,
        this->MeshData.Triangles,
        this->MeshData.Normals,
        this->MeshData.UV0,
        this->MeshData.Colors,
        this->MeshData.Tangents,
        true
    );

    this->ProcMesh->CreateMeshSection(
        1,
        this->TranslucentMeshData.Vertices,
        this->TranslucentMeshData.Triangles,
        this->TranslucentMeshData.Normals,
        this->TranslucentMeshData.UV0,
        this->TranslucentMeshData.Colors,
        this->TranslucentMeshData.Tangents,
        true
    );

    return;
}

void AChunk::ClearMesh()
{
    this->VertexCount = 0;
    this->TranslucentVertexCount = 0;
    this->MeshData.Clear();
    this->TranslucentMeshData.Clear();
    return;
}

int AChunk::GetVoxelIndex(const FIntVector& LocalVoxelPosition)
{
    return LocalVoxelPosition.Z * AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE + LocalVoxelPosition.Y * AChunk::CHUNK_SIZE + LocalVoxelPosition.X;
}

int AChunk::GetVoxel(const FIntVector& LocalVoxelPosition) const
{
    // TODO
    //      So we must check here if out of bounds for the neighboring
    //      chunk and get, if existing, the voxel data from there.
    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        return EWorldVoxel::AirVoxel;
    }
    
    return this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)];
}

void AChunk::ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel)
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
