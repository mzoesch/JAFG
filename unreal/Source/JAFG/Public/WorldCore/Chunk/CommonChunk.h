// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ChunkMeshData.h"
#include "GameFramework/Actor.h"
#include "WorldCore/Chunk/ChunkStates.h"

#include "CommonChunk.generated.h"

JAFG_VOID

class AChunkMulticasterInfo;
class UMaterialSubsystem;
class UServerChunkWorldSettings;
class UVoxelSubsystem;
class UProceduralMeshComponent;
class UChunkGenerationSubsystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FChunkStateChangeSignature, const EChunkState::Type /* NewChunkState */)

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
    GENERATED_BODY()

public:

    explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual ~ACommonChunk(void) override;

protected:

    // AActor implementation
    virtual auto BeginPlay(void) -> void override;
    virtual auto EndPlay(const EEndPlayReason::Type EndPlayReason) -> void override;
    // ~AActor implementation

    /**
     * Sets up data that is valid throughout the lifetime of a chunk and needed for the chunk to be able to
     * generate itself.
     */
    virtual auto InitializeCommonStuff(void) -> void;

    /**
     * Use as a last resort only.
     * Will delete without any pity this chunk from the UWorld.
     * No cleanup will be done.
     * It will probably result in an application crash in the very near future.
     */
    virtual auto KillUncontrolled(void) -> void;
    virtual auto KillControlled(void) -> void;
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

                auto SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive = 0.0f) -> void;
    FORCEINLINE auto GetChunkPersistency(void) const -> EChunkPersistency::Type { return this->ChunkPersistency; }

private:

    EChunkState::Type          ChunkState = EChunkState::Invalid;
    FChunkStateChangeSignature ChunkStateChangeEvent = FChunkStateChangeSignature();

    FDelegateHandle PrivateStateHandle = FDelegateHandle();
    auto SubscribeWithPrivateStateDelegate(void) -> void;

    EChunkPersistency::Type ChunkPersistency = EChunkPersistency::Persistent;
    TFuture<void>           PersistencyFuture;
    /** Counts how many persistent futures are currently active. */
    FThreadSafeCounter PersistentFutureCounter = 0;

protected:

    virtual bool IsStateChangeValid(const EChunkState::Type NewChunkState);

    virtual auto OnSpawned(void) -> void;
    virtual auto OnShaped(void) -> void;
    virtual auto OnSurfaceReplaced(void) -> void;
    virtual auto OnActive(void) -> void;
    virtual auto OnPendingKill(void) -> void;
    virtual auto OnKill(void) -> void;
    virtual auto OnBlockedByHyperlane(void) -> void;

    /** @return True, if state change was accepted. */
    virtual auto SetChunkState(const EChunkState::Type NewChunkState, const bool bForce = false) -> bool;
    friend UChunkGenerationSubsystem;

#pragma endregion Chunk State

#pragma region MISC

    //////////////////////////////////////////////////////////////////////////
    // MISC
    //////////////////////////////////////////////////////////////////////////

    /**
     * The chunk position in the world based on the J coordinate system.
     * Meaning the Chunk Key multiplied by the Chunk Size.
     * This should actually be a FIntVector, but FVector is more convenient to work with because we often
     * use this in calculations with floats. Use the ACommonChunk#ChunkPosition when calculating with other integers.
     * Be aware:
     * Always, when converting to any kind of J-Coordinate, be aware of flooring. IEEE 754 floating point precision
     * errors are no joke, they exist, never deny their unwanted existence, and there is definitely no fun in debugging
     * them. !!!Always make sure to round then!!!
     * (This variable is based on the actual AActor location that's why we have some float precision errors here.)
     */
    FVector      JChunkPosition = FVector::ZeroVector;
    FJCoordinate ChunkPosition  = FJCoordinate::ZeroValue;
    FChunkKey    ChunkKey       = FChunkKey::ZeroValue;
    /**
     * Generate the Chunk Key on the fly.
     * Usefully if this chunk is in the state of PreSpawned but the Chunk Key is needed.
     */
    auto GetChunkKeyOnTheFly(void) const -> FChunkKey;

    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    UPROPERTY()
    TObjectPtr<UChunkGenerationSubsystem> ChunkGenerationSubsystem = nullptr;

    UPROPERTY()
    TObjectPtr<AChunkMulticasterInfo> ChunkMulticasterInfo = nullptr;

    UPROPERTY()
    TObjectPtr<UMaterialSubsystem> MaterialSubsystem = nullptr;

    /** As the name suggests only valid on the server or in a standalone game. */
    UPROPERTY()
    TObjectPtr<UServerChunkWorldSettings> ServerChunkWorldSettings = nullptr;

#pragma endregion MISC

#pragma region Procedural Mesh

    //////////////////////////////////////////////////////////////////////////
    // Procedural Mesh
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent = nullptr;

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

    /**
     * If this chunk has been pre-initialized, this array will have a size of WorldStatics::ChunkSize to the
     * power of three. Not initialized at object construction to save memory.
     * This object will always be the owner of this array.
     */
    voxel_t* RawVoxelData = nullptr;

    /** It is up to the client and the derived class on them on how to feed this array. */
    TArray<FChunkMeshData> MeshData = TArray<FChunkMeshData>();
    /** It is up to the client and the derived class on them on how to feed this array. */
    TArray<int32>          VertexCounts = TArray<int32>();

    FORCEINLINE auto GetRawVoxelData(const FVoxelKey& LocalVoxelLocation) const -> voxel_t
    {
        return this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelLocation)];
    }

    /* Never make this const, please?? We are freaking changing the voxels. */
    // ReSharper disable once CppMemberFunctionMayBeConst
    FORCEINLINE void ModifyRawVoxelData(const FVoxelKey& LocalVoxelLocation, const voxel_t NewVoxel)
    {
        this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelLocation)] = NewVoxel;
    }

    /**
     * Gets the index of a voxel in the Raw Voxel Data array.
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

#pragma region Interaction

public:

    //////////////////////////////////////////////////////////////////////////
    // Interaction
    //////////////////////////////////////////////////////////////////////////

    /**
     * Server only.
     *
     * Fully replicated method to modify a single voxel with everything that is necessary as a side effect (e.g.: Mesh
     * re-rendering). Must never be called on the client. This is the safest way to modify a voxel from outside.
     * Must never be called in a large quantity or to modify the bulk data of this chunk as this method is expensive.
     *
     * @param LocalVoxelKey The local voxel key of within the chunk. Can be out of bounds of the chunk, but the method
     *                      will assume that the initial called chunk object acts as the pivot of the local voxel key.
     */
    void ModifySingleVoxel(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel);

    /**
     * Client only.
     *
     * Will predict the voxel modification on the client side while waiting for the server to confirm the modification.
     * This will prevent client side lag and will make the game feel more responsive when a high latency to the current
     * host is present.
     *
     * @param LocalVoxelKey The local voxel key of within the chunk. Can be out of bounds of the chunk, but the method
     *                      will assume that the initial called chunk object acts as the pivot of the local voxel key.
     */
    void PredictSingleVoxelModification(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel);

    /**
     * Client only.
     *
     * @param LocalVoxelKey Different from ACommonChunk#ModifySingleVoxel. Must be inside the defined bounds of the
     *                      chunk. And will crash if not.
     */
    void ModifySingleVoxelOnClient(const FVoxelKey& LocalVoxelKey, const voxel_t NewVoxel);

#pragma endregion Interaction

#pragma region Replication

    //////////////////////////////////////////////////////////////////////////
    // Replication
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto SendDataToClient(const TFunction<void(FPlatformTypes::uint32* VoxelData)>& Callback) const -> void
    {
        check( this->RawVoxelData != nullptr )
        Callback(this->RawVoxelData);
    }

    void SetInitializationDataFromAuthority(voxel_t* Voxels);

#pragma endregion Replication

#pragma region Getters

private:

    /** Never call directly. Always use the public getter methods. */
    auto GetChunkByNonZeroOrigin_Implementation(const FVoxelKey& LocalVoxelKey, FVoxelKey& OutTransformedLocalVoxelKey) -> ACommonChunk*;

public:

    FORCEINLINE const FChunkKey& GetChunkKey(void) const
    {
        return this->ChunkKey;
    }

    /**
     * Does not check for out of bounds. The callee must ensure that the
     * LocalVoxelPosition is within the bounds of the chunk.
     */
    [[nodiscard]]
    FORCEINLINE voxel_t GetLocalVoxelOnly(const FVoxelKey& LocalVoxelPosition) const
    {
        /*
         * TODO
         *      We should not have this if statement because of performance reasons. But currently we have to keep
         *      this as the greedy algorithm will often call this method with out of bounds values.
         *      This is a temporary solution.
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

    /**
     * TODO DO NOT USE in critical system areas. As this method currently contains bugs.
     *
     * Server only.
     *
     * Gets itself if inbounds or an existing outbounding chunk from the relative local voxel
     * position.
     *
     * Warning: This method is extremely slow.
     *          Large amounts of calls in one frame will cause a significant performance hit.
     *
     * @param LocalVoxelKey               The local voxel position within the chunk. This chunk acts as the
     *                                    pivot zero origin of the FIntVector.
     * @param OutTransformedLocalVoxelKey The new local voxel position of the target chunk. Stays the same if the
     *                                    target chunk is the called chunk.
     */
    auto GetChunkByNonZeroOrigin_Auth(const FVoxelKey& LocalVoxelKey, FVoxelKey& OutTransformedLocalVoxelKey) -> ACommonChunk*;
    /** Client implementation of the ACommonChunk#GetChunkByNonZeroOrigin_Auth. Only callable on a client. */
    auto GetChunkByNonZeroOrigin_Client(const FVoxelKey& LocalVoxelKey, FVoxelKey& OutTransformedLocalVoxelKey) -> ACommonChunk*;

    auto GetVoxelByNonZeroOrigin_Auth(const FVoxelKey& InLocalVoxelKey, voxel_t& OutVoxel) -> bool;

#pragma endregion Getters

};
