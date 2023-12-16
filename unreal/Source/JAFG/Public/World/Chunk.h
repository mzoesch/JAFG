// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "World/Chunks/ChunkMeshData.h"
#include "World/Voxel.h"

#include "Chunk.generated.h"

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
	void ApplyMesh() const;
	void ClearMesh();

protected:

	virtual void Setup() PURE_VIRTUAL(AChunk::Setup);
	virtual void InitiateVoxels() PURE_VIRTUAL(AChunk::InitiateBlocks);
	virtual void GenerateMesh() PURE_VIRTUAL(AChunk::GenerateMesh);
	virtual void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const EVoxel Voxel) PURE_VIRTUAL(AChunk::ModifyVoxelData);

public:

	TObjectPtr<UMaterialInterface> Material;
	
protected:

	TObjectPtr<UProceduralMeshComponent> Mesh;
	FastNoiseLite* Noise;
	FChunkMeshData MeshData;
	int VertexCount = 0;

public:

	void ModifyVoxel(const FIntVector& LocalVoxelPosition, const EVoxel Voxel);
	
};
