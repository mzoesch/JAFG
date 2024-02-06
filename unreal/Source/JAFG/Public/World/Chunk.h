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
    
    void RegenerateMesh();

    /** The mesh of the chunk is not re generated. */
    FORCEINLINE void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const int Voxel) { this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = Voxel; }

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
    void DWReplaceSurface();
    /* Caves */
    /* Features and structures */
    void AddTrees();
    void DWAddFeaturesAndStructures();
    
protected:

    UPROPERTY()
    AChunkWorld* ChunkWorld;

    FVector     ChunkPosition;
    FIntVector  ChunkKey;

    FORCEINLINE FIntVector GetTopChunkKey() const { return FIntVector(this->ChunkKey.X, this->ChunkKey.Y, this->ChunkKey.Z + 1); }
    FORCEINLINE FIntVector GetBottomChunkKey() const { return FIntVector(this->ChunkKey.X, this->ChunkKey.Y, this->ChunkKey.Z - 1); }

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

    /**
     * Gets itself if inbounds or an existing other chunk from the relative local voxel
     * position that is also a neighbor from the called chunk.
     *
     * Warning: This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     */
    AChunk* GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition);
    int GetVoxel(const FIntVector& LocalVoxelPosition) const;
    /** 
     * This only works for the called chunk and all other chunks neighboring that chunk.
     * But never for chunks that are not neighbors.
     *
     * Warning: This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     */
    void ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel);
    /**
     * Faster implementation of ModifyVoxel. But only checks for out of bounds in the top direction.
     * No safety checks for other directions.
     */
    void ModifyVoxelCheckTopChunkOnly(const FIntVector& CallingChunkLocalVoxelPosition, const int Voxel);
};
