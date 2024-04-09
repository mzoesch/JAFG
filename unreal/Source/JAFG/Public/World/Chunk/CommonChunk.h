// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/Actor.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Voxel/CommonVoxels.h"
#include "World/Voxel/VoxelSubsystem.h"

#include "CommonChunk.generated.h"

JAFG_VOID

class AWorldPlayerController;
class UProceduralMeshComponent;
struct FChunkMeshData;
struct FInitialChunkData;

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
    GENERATED_BODY()

public:

    explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    /* Note to self, Nanite with static meshes on far away chunks. */

    /**
     * Sets up data that is valid throughout the lifetime of a chunk and needed for the chunk to be able to
     * generate itself.
     */
    void PreInitialize(void);

protected:

    //////////////////////////////////////////////////////////////////////////
    // Procedural Mesh
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

    /**
     * Must be overriden by the derived class to generate the procedural mesh.
     */
    virtual void GenerateProceduralMesh(void) PURE_VIRTUAL(ACommonChunk::GenerateProceduralMesh)
    void ApplyProceduralMesh(void) const;
    /* make inline? */
    void ClearMesh(void);

    //////////////////////////////////////////////////////////////////////////
    // Raw Data
    //////////////////////////////////////////////////////////////////////////

    /**
     * Can we change the int type?
     */
    TArray<int32> RawVoxels;

    FORCEINLINE void ModifyRawVoxelData(const FIntVector& LocalVoxelPosition, const int NewVoxel)
    {
        this->RawVoxels[ACommonChunk::GetVoxelIndex(LocalVoxelPosition)] = NewVoxel;
    }

    /** It is up to the client on how to feed these arrays. */
    TArray<FChunkMeshData> MeshData;
    TArray<int> VertexCounts; /* Can we change the int? */

    //////////////////////////////////////////////////////////////////////////
    // MISC
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY()
    TObjectPtr<AWorldGeneratorInfo> WorldGeneratorInfo;
    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem;

    /** TODO Is this correct?
     * Should be actually an FIntVector.
     * But because we often calculate with floats we use the normal
     * FVector so that we do not have to cast all the time.
     */
    FVector JChunkPosition;
    FIntVector ChunkKey;

    FORCEINLINE static int GetVoxelIndex(const FIntVector& LocalVoxelPosition)
    {
        return
            LocalVoxelPosition.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize +
            LocalVoxelPosition.Y * AWorldGeneratorInfo::ChunkSize +
            LocalVoxelPosition.X;
    }

private:

    //////////////////////////////////////////////////////////////////////////
    // Chunk World Generation
    //////////////////////////////////////////////////////////////////////////

    void GenerateVoxels(void);

    void GenerateSuperFlatWorld(void);

public:

    //////////////////////////////////////////////////////////////////////////
    // Server Client Interaction
    //////////////////////////////////////////////////////////////////////////

    /**
     * Server only.
     *
     * Called to package the initial chunk data that is needed to initialize the chunk on the client and send it to the
     * targeted client via the Hyperlane.
     */
    void SendInitializationDataToClient(AWorldPlayerController* Target) const;

    /**
     * Client only.
     *
     * Called to override all voxels in the chunk with the given data and generate the procedural mesh accordingly.
     */
    void InitializeWithAuthorityData(const TArray<int32>& InRawVoxels);

    //////////////////////////////////////////////////////////////////////////
    // Public interaction
    //////////////////////////////////////////////////////////////////////////

    /**
     * Server only.
     *
     * Gets itself if inbounds or an existing outbounding chunk from the relative local voxel
     * position that is also a neighbor from this (the called) chunk.
     *
     * Warning: This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     */
    ACommonChunk* GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition);

    /*
     * Server only.
     *
     * Safely changes a single voxel at the given local voxel position and causes a complete mesh rerender on all
     * clients.
     *
     * Warning:
     * This method is extremely slow.
     * Large amounts of calls in one frame will cause a significant performance hit.
     *
     * @param LocalVoxelPosition The local voxel position within the chunk. Can be out of bounds of the chunk but
     *                           the method will assume that the initial called chunk object acts as the pivot of the
     *                           FIntVector.
     */
    void ModifySingleVoxel(const FIntVector& LocalVoxelPosition, int NewVoxel);

    //////////////////////////////////////////////////////////////////////////
    // Getters
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE const FIntVector& GetChunkKey(void) const
    {
        return this->ChunkKey;
    }

    /**
     * Does not check for out of bounds. The callee must ensure that the
     * LocalVoxelPosition is within the bounds of the chunk.
     */
    /* TODO Can we change the int type */
    FORCEINLINE int GetLocalVoxelOnly(const FIntVector& LocalVoxelPosition) const
    {
        /*
         * Do we need to have this if statement?
         */
        if (
            LocalVoxelPosition.X >= AWorldGeneratorInfo::ChunkSize
            || LocalVoxelPosition.Y >= AWorldGeneratorInfo::ChunkSize
            || LocalVoxelPosition.Z >= AWorldGeneratorInfo::ChunkSize
            || LocalVoxelPosition.X < 0
            || LocalVoxelPosition.Y < 0
            || LocalVoxelPosition.Z < 0
        )
        {
            return ECommonVoxels::Air;
        }

        return this->RawVoxels[ACommonChunk::GetVoxelIndex(LocalVoxelPosition)];
    }

    /**
     * Converts any unreal vector to a local voxel int location.
     * XYZ reaching from zero to AWorldGeneratorInfo::ChunkSize.
     */
    static FIntVector WorldToLocalVoxelLocation(const FVector& WorldLocation);
};
