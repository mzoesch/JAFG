// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Voxel/CommonVoxels.h"
#include "World/Voxel/VoxelSubsystem.h"

#include "CommonChunk.generated.h"

struct FChunkMeshData;
class UProceduralMeshComponent;

UCLASS(Abstract, NotBlueprintable)
class JAFG_API ACommonChunk : public AActor
{
	GENERATED_BODY()

public:

	explicit ACommonChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void BeginPlay(void) override;

private:

	/* Nanite with static meshes on far away chunks. */

	void Initialize(void);
	void GenerateVoxels(void);
protected: /** The derived classes should implement this method. */
	virtual void GenerateProceduralMesh(void) PURE_VIRTUAL(ACommonChunk::GenerateProceduralMesh)
private:
	void ApplyProceduralMesh(void) const;
	FORCEINLINE void ClearMesh(void)
	{
		/* TODO We might want to use arr.empty? */
	}
	
protected:

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;
	
	//////////////////////////////////////////////////////////////////////////
	// Raw Data
	//////////////////////////////////////////////////////////////////////////

	/* Can we change the int? */
	TArray<int> RawVoxels;
	TArray<FChunkMeshData> MeshData;
	TArray<int> VertexCounts; /* Can we change the int? */

	//////////////////////////////////////////////////////////////////////////
	// MISC
	//////////////////////////////////////////////////////////////////////////
	
	UPROPERTY()
 	TObjectPtr<AWorldGeneratorInfo> WorldGeneratorInfo;
	UPROPERTY()
	TObjectPtr<UVoxelSubsystem>     VoxelSubsystem;

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
