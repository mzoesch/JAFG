// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Voxel/CommonVoxels.h"
#include "World/Voxel/VoxelSubsystem.h"

#include "CommonChunk.generated.h"

struct FInitialChunkData;
class AJAFGPlayerController;
class UBackgroundChunkUpdaterComponent;
class UProceduralMeshComponent;
struct FChunkMeshData;

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
	GENERATED_BODY()

	friend UBackgroundChunkUpdaterComponent;
	friend AJAFGPlayerController;
	
public:

	explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	
	/**
	 * Must only be called on clients never on a server or standalone.
	 */
	void PreClientBeginPlay(void);
	bool bInitializedClientBeginPlay = false;
	virtual void BeginPlay(void) override;

private:

	/* Nanite with static meshes on far away chunks. */

	void Initialize(void);
	void GenerateVoxels(void);

protected: /** The derived classes should implement this method. */

	virtual void GenerateProceduralMesh(void) PURE_VIRTUAL(ACommonChunk::GenerateProceduralMesh)

private:

	UFUNCTION()
	void OnRep_RawVoxels();
	void ApplyProceduralMesh(void) const;
	void ClearMesh(void);
	
	void FillDataFromAuthorityAsync(void);
	FInitialChunkData MakeInitialChunkData(void) const;
	
	// /**
	//  * Called from a client to get the initial voxel data from the server that was generated on it.
	//  */
	// UFUNCTION(Server, Reliable)
	// void FillDataWithAuthorityData_ServerRPC();
	// /**
	//  * Called from the server to set minimal initial voxel data on the client. So that the chunk can be generated
	//  * on the client.
	//  */
	// UFUNCTION(Client, Reliable)
	// void FillDataWithAuthorityData_ClientRPC(const FInitialMinimalVoxelData& InitialMinimalVoxelData);
	
	/**
	 * Must be called on the client during the Begin Play phase as some data may shrink after initialization on the
	 * client if it is not directly used.
	 * This is kind of a workaround and later on we should look into the proper way to handle this. Disallow shrinking
	 * in the first place.
	 */
	void PreventRawDataMemoryShrinking(void);
	
protected:

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;
	
	//////////////////////////////////////////////////////////////////////////
	// Raw Data
	//////////////////////////////////////////////////////////////////////////

	// ftcplistener 
	
	/* Can we change the int? */
	UPROPERTY(ReplicatedUsing=OnRep_RawVoxels)
	TArray<int> RawVoxels;

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
	UPROPERTY()
	TObjectPtr<UBackgroundChunkUpdaterComponent> BCHC;
	
    /** TODO Is this correct?
     * Should be actually an FIntVector.
     * But because we often calculate with floats we use the normal
     * FVector so that we do not have to cast all the time.
     */
	FVector    JChunkPosition;
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
