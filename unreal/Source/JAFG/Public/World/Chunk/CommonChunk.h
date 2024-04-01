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
    
    /* Nanite with static meshes on far away chunks. */

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
     * Maybe we can take a look at TTcpListener later on.
     * 
     * Can we change the int type?
     */
    UPROPERTY(ReplicatedUsing=OnRep_RawVoxels)
    TArray<int32> RawVoxels;

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
};
