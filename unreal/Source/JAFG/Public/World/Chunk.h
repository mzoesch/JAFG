// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "World/Chunks/ChunkMeshData.h"

#include "Chunk.generated.h"

class AChunkWorld;
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
    
    void Setup();
    void GenerateVoxels();
    void ApplyMesh() const;
    void ClearMesh();

    void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const int Voxel);

protected:

    virtual void GenerateMesh() PURE_VIRTUAL(AChunk::GenerateMesh);

protected:

    /* MISC */

    static int GetVoxelIndex(const FIntVector& LocalVoxelPosition);

private:

    /* Chunk World Generation */

    void GenerateSuperFlatWorld();
    void GenerateDefaultWorld();

    /* Default World Generation */
    void DWShapeTerrain();
    /* Water Filling */
    void DWSurfaceReplacement();
    /* Caves */
    /* Features and structrues */
    
protected:

    UPROPERTY()
    AChunkWorld* ChunkWorld;

    FVector ActorCoordinate;
    
protected:

    bool bGenerationFailed;
    
    /* Data */
    TArray<int> Voxels;
    
    /* Mesh */
    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProcMesh;

    TArray<FChunkMeshData>  MeshDataArray;
    TArray<int>             VertexCounts;

public:

    int GetVoxel(const FIntVector& LocalVoxelPosition) const;
    void ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel);
    
};
