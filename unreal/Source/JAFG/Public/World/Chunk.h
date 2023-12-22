// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "World/Chunks/ChunkMeshData.h"
#include "World/Voxel.h"

#include "Chunk.generated.h"

// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class JAFG_API AChunk : public AActor
{
    GENERATED_BODY()

public:

    AChunk();

public:

    static constexpr int CHUNK_SIZE{32};

protected:

    virtual void BeginPlay() override;

private:

    /* TODO Nanite with static meshes on far away chunks. */
    void PreSetup();
    void ApplyMesh() const;
    void ClearMesh();

protected:
    
    virtual void Setup() PURE_VIRTUAL(AChunk::Setup);
    virtual void InitiateVoxels() PURE_VIRTUAL(AChunk::InitiateBlocks);
    virtual void GenerateMesh() PURE_VIRTUAL(AChunk::GenerateMesh);
    virtual void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const EVoxel Voxel) PURE_VIRTUAL(AChunk::ModifyVoxelData);

protected:

    /* MISC */
    static int GetVoxelIndex(const FIntVector& LocalVoxelPosition);
    
protected:

    /* Data */
    TArray<EVoxel> Voxels;
    
    /* Mesh */
    TObjectPtr<UProceduralMeshComponent> Mesh;
    FastNoiseLite* Noise;
    FChunkMeshData MeshData;
    FChunkMeshData TranslucentMeshData;
    int VertexCount = 0;
    int TranslucentVertexCount = 0;

public:

    EVoxel GetVoxel(const FIntVector& LocalVoxelPosition) const;
    void ModifyVoxel(const FIntVector& LocalVoxelPosition, const EVoxel Voxel);
    
};
