// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ChunkMeshData.h"
#include "GameFramework/Actor.h"
#include "WorldCore/Chunk/ChunkStates.h"

#include "CommonChunk.generated.h"

JAFG_VOID

class UServerChunkWorldSettings;
class UVoxelSubsystem;
class UProceduralMeshComponent;
class UChunkGenerationSubsystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FChunkStateChangeSignature, const EChunkState::Type /* NewChunkState */ )

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
    GENERATED_BODY()

public:

    explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual auto BeginPlay(void) -> void override;
    virtual auto EndPlay(const EEndPlayReason::Type EndPlayReason) -> void override;
    // ~AActor implementation

    /**
     * Sets up data that is valid throughout the lifetime of a chunk and needed for the chunk to be able to
     * generate itself.
     * Called before ACommonChunk#Initialize.
     */
    virtual auto PreInitialize(void) -> void;

    /**
     * Use as a last resort only.
     * Will delete without any pity this chunk from the UWorld.
     * No cleanup will be done.
     */
    virtual auto KillUncontrolled(void) -> void;
    bool bUncontrolledKill = false;

#pragma region Chunk State

    //////////////////////////////////////////////////////////////////////////
    // Chunk State
    //////////////////////////////////////////////////////////////////////////

public:

    FORCEINLINE auto SubscribeToChunkStateChange(const FChunkStateChangeSignature::FDelegate& Delegate) -> FDelegateHandle
    {
        return this->ChunkStateChangeEvent.Add(Delegate);
    }

    FORCEINLINE auto UnsubscribeFromChunkStateChange(const FDelegateHandle& Handle) -> bool
    {
        return this->ChunkStateChangeEvent.Remove(Handle);
    }

    FORCEINLINE auto GetChunkState(void) const -> EChunkState::Type
    {
        return this->ChunkState;
    }

private:

    EChunkState::Type          ChunkState = EChunkState::Invalid;
    FChunkStateChangeSignature ChunkStateChangeEvent;

    FDelegateHandle SpawnedHandle;
    FDelegateHandle ShapedHandle;
    FDelegateHandle SurfaceReplacedHandle;

    FDelegateHandle ActiveHandle;

    FDelegateHandle CreateOnSpawnedDelegateHandle(void);
    FDelegateHandle CreateOnShapedDelegateHandle(void);
    FDelegateHandle CreateOnSurfaceReplacedDelegateHandle(void);
    FDelegateHandle CreateOnActiveDelegateHandle(void);

protected:

    virtual void SetChunkState(const EChunkState::Type NewChunkState);
    friend UChunkGenerationSubsystem;

#pragma endregion Chunk State

#pragma region MISC

    //////////////////////////////////////////////////////////////////////////
    // MISC
    //////////////////////////////////////////////////////////////////////////

    // TODO Check what this value can be in edge cases
    FVector   JChunkPosition;
    FChunkKey ChunkKey;

    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem;

    /** As the name suggests only valid on the server or in a standalone game. */
    UPROPERTY()
    TObjectPtr<UServerChunkWorldSettings> ServerChunkWorldSettings;

#pragma endregion MISC

#pragma region Procedural Mesh

    //////////////////////////////////////////////////////////////////////////
    // Procedural Mesh
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

    virtual auto GenerateProceduralMesh(void) -> void PURE_VIRTUAL(ACommonChunk::GenerateProceduralMesh)
            auto ApplyProceduralMesh(void) -> void;

    FORCEINLINE auto ClearProceduralMesh(void) -> void
    {
        for (FChunkMeshData& ConcreteMeshData : this->MeshData)
        {
            ConcreteMeshData.Clear();
        }

        for (int32& VertexCount : this->VertexCounts)
        {
            VertexCount = 0;
        }

        return;
    }

    FORCEINLINE auto RegenerateProceduralMesh(void) -> void
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh();
        this->ApplyProceduralMesh();

        return;
    }

    //////////////////////////////////////////////////////////////////////////
    // Raw Data
    //////////////////////////////////////////////////////////////////////////

    TArray<voxel_t> RawVoxelData;

    /** It is up to the client and the derived class on them on how to feed this array. */
    TArray<FChunkMeshData> MeshData;
    /** It is up to the client and the derived class on them on how to feed this array. */
    TArray<int32>          VertexCounts;

#if !UE_BUILD_SHIPPING
    /**
     * Never actually use in production code. This is just a helper method for developing and testing. Often things
     * may go wrong while developing the generation system, and this method helps to not crash the game completely
     * if something goes wrong. It will show an instantly recognizable pattern in the wrong chunk to immediately see
     * that something went wrong with the voxel generation due to a Hyperlane error or the generation algorithm itself.
     *
     * A nice side effect is that it will hold the auto shrinking system in the TArray when it is initialized with zero
     * only values.
     */
    FORCEINLINE void HoldAutoShrinking(void)
    {
        for (int i = 0; i < FMath::Pow(WorldStatics::ChunkSize, 3.0f); ++i)
        {
            this->RawVoxelData[i] = i % 2 == 0 ? ECommonVoxels::GetBaseVoxel() : ECommonVoxels::Air;
        }

        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    FORCEINLINE auto GetRawVoxelData(const FVoxelKey& LocalVoxelLocation) const -> voxel_t
    {
        return this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelLocation)];
    }

    FORCEINLINE void ModifyRawVoxelData(const FVoxelKey& LocalVoxelLocation, const voxel_t NewVoxel)
    {
        this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelLocation)] = NewVoxel;
    }

    /**
     * Gets the index of a voxel in the RawVoxelData array.
     */
    FORCEINLINE static auto GetVoxelIndex(const FVoxelKey LocalVoxelLocation) -> uint16
    {
        return
            LocalVoxelLocation.Z * WorldStatics::ChunkSize * WorldStatics::ChunkSize +
            LocalVoxelLocation.Y * WorldStatics::ChunkSize +
            LocalVoxelLocation.X;
    }

#pragma endregion Procedural Mesh

#pragma region Chunk World Generation

private:

    //////////////////////////////////////////////////////////////////////////
    // Chunk World Generation
    //////////////////////////////////////////////////////////////////////////

    void Shape(void);
    void GenerateSuperFlatWorld(void);
    void GenerateDefaultWorld(void);

    void ReplaceSurface(void);
    void GenerateSurface(void);

#pragma endregion Chunk World Generation

#pragma region Getters

public:

    FORCEINLINE const FChunkKey& GetChunkKey(void) const
    {
        return this->ChunkKey;
    }

    /**
     * Does not check for out of bounds. The callee must ensure that the
     * LocalVoxelPosition is within the bounds of the chunk.
     */
    FORCEINLINE voxel_t GetLocalVoxelOnly(const FVoxelKey& LocalVoxelPosition) const
    {
        /*
         * Do we need to have this if statement?
         */
        if (
               LocalVoxelPosition.X >= WorldStatics::ChunkSize
            || LocalVoxelPosition.Y >= WorldStatics::ChunkSize
            || LocalVoxelPosition.Z >= WorldStatics::ChunkSize
            || LocalVoxelPosition.X < 0
            || LocalVoxelPosition.Y < 0
            || LocalVoxelPosition.Z < 0
        )
        {
            return ECommonVoxels::Air;
        }

        return this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelPosition)];
    }

#pragma endregion Getters

};
