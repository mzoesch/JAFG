// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "ChunkArena.h"
#include "MyCore.h"
#include "ChunkMeshData.h"
#include "GameFramework/Actor.h"
#include "WorldCore/Chunk/ChunkStates.h"
#include "WorldCore/Chunk/CommonChunkParmas.h"

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
class JAFG_API ACommonChunk : public AActor, public IChunkArenaGladiator
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

    // IChunkArenaGladiator interface
                virtual auto OnAllocate(const FChunkKey& InChunkKey) -> void override;
                virtual auto OnFree(void) -> void override;
                virtual auto OnObliterate(void) -> void override;
    FORCEINLINE virtual auto GetChunkKeyOnTheFly_Gladiator(void) const -> FChunkKey override { return this->GetChunkKeyOnTheFly(true); }
    FORCEINLINE virtual auto AsChunk(void) const -> const ACommonChunk* override { return this; }
    FORCEINLINE virtual auto AsChunk(void) -> ACommonChunk* override { return this; }
    // ~IChunkArenaGladiator interface

    /**
     * Sets up data that is valid throughout the lifetime of a chunk and needed for the chunk to be able to
     * generate itself.
     */
    virtual auto InitializeCommonStuff(void) -> void;

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
    FORCEINLINE auto IsPersistent(void) const -> bool { return this->ChunkPersistency == EChunkPersistency::Persistent; }
    FORCEINLINE auto IsTemporary(void) const -> bool { return this->ChunkPersistency == EChunkPersistency::Temporary; }
    FORCEINLINE auto ShouldBeKilled(void) const -> bool
    {
        return this->IsTemporary() && this->RealTimeInSecondsWhenTemporaryChunkShouldBeKilled < this->GetWorld()->GetRealTimeSeconds();
    }

private:

    EChunkState::Type          ChunkState = EChunkState::Invalid;
    FChunkStateChangeSignature ChunkStateChangeEvent = FChunkStateChangeSignature();

    FDelegateHandle PrivateStateHandle = FDelegateHandle();
    auto SubscribeWithPrivateStateDelegate(void) -> void;

    EChunkPersistency::Type ChunkPersistency = EChunkPersistency::Persistent;
    /**
     * The real time (not stopped or dilated / clamped) when this chunk should be killed by the generation subsystem.
     * Only meaningful when the persistency of this chunk is temporary.
     */
    double RealTimeInSecondsWhenTemporaryChunkShouldBeKilled = 0.0f;

protected:

    bool IsStateChangeValid(const EChunkState::Type NewChunkState) const;

    auto OnSpawned(void) -> void;
    auto OnShaped(void) -> void;
    auto OnSurfaceReplaced(void) -> void;
    auto OnShapedCaves(void) -> void;
    auto OnActive(void) -> void;
    auto OnPendingKill(void) -> void;
    auto OnKill(void) -> void;
    auto OnBlockedByHyperlane(void) const -> void;

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
     * use this in calculations with floats.
     * Be aware:
     * Always, when converting to any kind of J-Coordinate, be aware of flooring. IEEE 754 floating point precision
     * errors are no joke, they exist, never deny their unwanted existence, and there is definitely no fun in debugging
     * them. !!!Always make sure to round then!!!
     * (This variable is based on the actual AActor location that's why we have some float precision errors here.)
     */
    FVector      JChunkPosition = FVector::ZeroVector;
    FChunkKey    ChunkKey       = FChunkKey::ZeroValue;
    /**
     * Generate the Chunk Key on the fly.
     * Usefully if this chunk is in the state of PreSpawned but the Chunk Key is needed.
     */
    auto GetChunkKeyOnTheFly(const bool bAllowNonZeroOnMember = false) const -> FChunkKey;

    FSharedChunkParams* ChunkParams = nullptr;
    FORCEINLINE auto GetChunkParams(void) const -> FSharedChunkParams* { return this->ChunkParams; }
    FORCEINLINE auto GetChunkParamsChecked(void) const -> FSharedChunkParams* { jcheck(this->ChunkParams); return this->ChunkParams; }
    FORCEINLINE auto GetVoxelSubsystem(void) const -> UVoxelSubsystem* { return this->ChunkParams->VoxelSubsystem; }
    FORCEINLINE auto GetChunkGenerationSubsystem(void) const -> UChunkGenerationSubsystem* { return this->ChunkParams->ChunkGenerationSubsystem; }
    FORCEINLINE auto GetChunkMulticasterInfo(void) const -> AChunkMulticasterInfo* { return this->ChunkParams->ChunkMulticasterInfo; }
    FORCEINLINE auto GetMaterialSubsystem(void) const -> UMaterialSubsystem* { return this->ChunkParams->MaterialSubsystem; }
    FORCEINLINE auto GetServerChunkWorldSettings(void) const -> UServerChunkWorldSettings* { return this->ChunkParams->ServerChunkWorldSettings; }

    /*
     * Kinda annoying these warning. But with the generation subsystem, we keep track of deleted chunks.
     * We definitely do not want to have any ownership of these chunks in any way.
     */

    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NNorth;
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NEast;
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NSouth;
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NWest;
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NUp;
    // ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
    ACommonChunk* NDown;

#pragma endregion MISC

#pragma region Procedural Mesh

    //////////////////////////////////////////////////////////////////////////
    // Procedural Mesh
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent = nullptr;

    virtual auto GenerateProceduralMesh(void) -> void;
    virtual auto GenerateCollisionConvexMesh(void) -> void;
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
    void Shape_SuperFlat(void);
    void Shape_Default(void);

    void ReplaceSurface(void);
    void ReplaceSurface_SuperFlat(void);
    void ReplaceSurface_Default(void);

    void ShapeCaves(void);
    void ShapeCaves_SuperFlat(void);
    void ShapeCaves_Default(void);

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

    FORCEINLINE auto GetChunkKey(void) const -> const FChunkKey& { return this->ChunkKey; }
    FORCEINLINE auto ToString(void) const -> FString { return this->GetChunkKey().ToString(); }

    /**
     * Does not check for out of bounds. The callee must ensure that the
     * input is within the bounds of the chunk.
     */
    FORCEINLINE auto GetLocalVoxelOnly(const FVoxelKey& LocalVoxelPosition) const -> voxel_t
    {
#if WITH_EDITOR
        /*
         * Compiling this out as this method heavily impacts performance
         * (One of the most important methods when generating the meshes).
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
            jrelaxedCheckNoEntry()
            return ECommonVoxels::Air;
        }
#endif /* WITH_EDITOR */

        if (this->RawVoxelData)
        {
            const voxel_t Out = this->RawVoxelData[ACommonChunk::GetVoxelIndex(LocalVoxelPosition)];
            return Out == ECommonVoxels::Null ? ECommonVoxels::Air : Out;
        }

        return ECommonVoxels::Air;
    }

    /**
     * Allows for one single one-off out-of-bounds direction.
     */
    FORCEINLINE auto GetFastLocalAndAdjacentVoxel(const FVoxelKey& VoxelKey) const -> voxel_t
    {
        if (VoxelKey.X < 0)
        {
            return this->NSouth
                ? this->NSouth->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X + WorldStatics::ChunkSize, VoxelKey.Y, VoxelKey.Z))
                : ECommonVoxels::Air;
        }

        if (VoxelKey.X >= WorldStatics::ChunkSize)
        {
            return this->NNorth
                ? this->NNorth->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X - WorldStatics::ChunkSize, VoxelKey.Y, VoxelKey.Z))
                : ECommonVoxels::Air;
        }

        if (VoxelKey.Y < 0)
        {
            return this->NWest
                ? this->NWest->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X, VoxelKey.Y + WorldStatics::ChunkSize, VoxelKey.Z))
                : ECommonVoxels::Air;
        }

        if (VoxelKey.Y >= WorldStatics::ChunkSize)
        {
            return this->NEast
                ? this->NEast->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X, VoxelKey.Y - WorldStatics::ChunkSize, VoxelKey.Z))
                : ECommonVoxels::Air;
        }

        if (VoxelKey.Z < 0)
        {
            return this->NDown
                ? this->NDown->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X, VoxelKey.Y, VoxelKey.Z + WorldStatics::ChunkSize))
                : ECommonVoxels::Air;
        }

        if (VoxelKey.Z >= WorldStatics::ChunkSize)
        {
            return this->NUp
                ? this->NUp->GetLocalVoxelOnly(FVoxelKey(VoxelKey.X, VoxelKey.Y, VoxelKey.Z - WorldStatics::ChunkSize))
                : ECommonVoxels::Air;
        }

        return this->GetLocalVoxelOnly(VoxelKey);
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
