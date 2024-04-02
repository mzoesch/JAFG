// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Voxel/CommonVoxels.h"
#include "World/Voxel/VoxelSubsystem.h"

#include "CommonChunk.generated.h"

class AJAFGPlayerController;
class UProceduralMeshComponent;
struct FChunkMeshData;
struct FInitialChunkData;

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
    GENERATED_BODY()

    friend AJAFGPlayerController;

public:

    explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

    /**
     * Variables may not have been initialized yet on the client.
     * This variable must never be true on any server type.
     */
    bool bInitializedClientBeginPlay = false;

    virtual void BeginPlay(void) override;

private:

    /* Note to self, Nanite with static meshes on far away chunks. */

    void Initialize(void);
    void GenerateVoxels(void);

protected:

    virtual void GenerateProceduralMesh(void) PURE_VIRTUAL(ACommonChunk::GenerateProceduralMesh)

private:

    UFUNCTION()
    void OnRep_RawVoxels();
    void ApplyProceduralMesh(void) const;
    void ClearMesh(void);

protected:

    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

    //////////////////////////////////////////////////////////////////////////
    // Raw Data
    //////////////////////////////////////////////////////////////////////////

    /**
     * Currently using the unreal replication system. But is this the best way? We may run into performance issues very
     * fast in the future. We may need to implement our own replication system.
     * Besides the main problem is that we are limited to a maximum Bunch size of 2^16 = 65.536 bytes. If we want to
     * have int32 arrays with a size of (32x32x32) * 4 bytes = 32.768 * 4 bytes = 131.072 bytes, we are already over
     * the limit.
     *
     * Have a look at PushMode.h for more information.
     *
     * Possible solutions:
     *
     * Custom UDP TCP socket system?
     * Maybe we can take a look at FTcpListener later on.
     * For anyone else who finds this, I’ve concluded that using RPC/Replication to send large amounts of data is not preferable so I’m gonna try using the built in UDP Socket system to send this information
     * FFastArraySerializer
     *
     * Can we change the int type?
     */
    UPROPERTY(ReplicatedUsing=OnRep_RawVoxels)
    TArray<int32> RawVoxels;

    FORCEINLINE void ModifyRawVoxelData(const FIntVector& LocalVoxelPosition, const int NewVoxel)
    {
        this->RawVoxels[ACommonChunk::GetVoxelIndex(LocalVoxelPosition)] = NewVoxel;
    }

    /* It is up to the client on how to feed these arrays. */
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

    void GenerateSuperFlatWorld(void);

public:

    //////////////////////////////////////////////////////////////////////////
    // Public interaction

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
