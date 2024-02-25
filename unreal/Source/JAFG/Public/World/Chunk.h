// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "World/Chunks/ChunkMeshData.h"

#include "Chunk.generated.h"

struct FVoxelMask;
class AChunkWorld;
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS(Abstract)
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

    /** The mesh of the chunk is not regenerated. */
    FORCEINLINE void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const int Voxel) { this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = Voxel; }

protected:

    virtual void GenerateMesh() PURE_VIRTUAL(AChunk::GenerateMesh);

protected:

    /* MISC */

    static int GetVoxelIndex(const FIntVector& LocalVoxelPosition);

private:

    //////////////////////////////////////////////////////////////////////////
    // Chunk World Generation
    //////////////////////////////////////////////////////////////////////////

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

    //////////////////////////////////////////////////////////////////////////
    //  Raw Data
    //////////////////////////////////////////////////////////////////////////
    
    bool bGenerationFailed;
    
    /* Raw Data */
    TArray<int> Voxels;
    
    /* Mesh */
    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProcMesh;

    TArray<FChunkMeshData>  MeshDataArray;
    TArray<int>             VertexCounts;

public:

    /**
     * Gets itself if inbounds or an existing outbounding chunk from the relative local voxel
     * position that is also a neighbor from this (the called) chunk.
     *
     * Warning: This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     */
    AChunk* GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition);
    /*
     * TODO THIS METHOD NEEDS SOME WORK
     * TODO FIND THE BEST USE-CASE AND CHECK IF ALL CALLS ARE ACTUALLY NEEDED
     * TODO AS THIS METHOD IS EXTREMELY SLOW
     */
    int GetVoxel(const FIntVector& LocalVoxelPosition) const;
    /** 
     * This only works for the called chunk and all other chunks neighboring that chunk.
     * But never for chunks that are not neighbors.
     *
     * Warning:
     * This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     *
     * If on world shaping or generation (or any other large scale task),
     * the direct array access is recommended by using the ModifyVoxelData method.
     */
    void ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel);
    /**
     * Faster implementation of ModifyVoxel. But only checks for outbounding chunks in the top direction.
     * No safety checks for other directions.
     */
    void ModifyVoxelCheckTopChunkOnly(const FIntVector& CallingChunkLocalVoxelPosition, const int Voxel);

    /* TODO For all other five directions. */
    
public:
    
    //////////////////////////////////////////////////////////////////////////
    // Non Generation Related
    //
    // All methods below are not meant to be called in a large amount of
    // calls in one tick. As they are not optimized for that. They are
    // meant to be called if a given event is triggered once in a while.
    //////////////////////////////////////////////////////////////////////////

    /**
     * Only checks for the inbounding chunk. Safety checks has to be done by the caller.
     * There will be undefined behavior if the local voxel position is out of bounds.
     *
     * @return True if the targeted voxel has a secondary input action character event, false otherwise.
     *         Note that there are no checks if the caller has a valid input mapping context to trigger such event.
     */
    bool HasCustomSecondaryCharacterEventVoxel(const FIntVector& LocalVoxelPosition, FVoxelMask& OutVoxelMask) const;

    // TODO Get Voxel Class if available
    
};
