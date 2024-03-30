// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/CommonChunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/MaterialSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/Chunk/ChunkMeshData.h"
#include "World/Voxel/CommonVoxels.h"

ACommonChunk::ACommonChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	this->PrimaryActorTick.bCanEverTick = false;

	this->ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	this->SetRootComponent(this->ProceduralMeshComponent);
	// We can do this for far away chunks maybe?
	/* this->ProceduralMeshComponent->bUseAsyncCooking = true; */
	this->ProceduralMeshComponent->SetCastShadow(true);

	return;
}

void ACommonChunk::BeginPlay(void)
{
	Super::BeginPlay();

   /*
    * To access some faster generation we might want to pass this as a reference by the generator.
    * There might be over a 100 calls per tick to this method when the generation is generation new chunks.
    */
	this->WorldGeneratorInfo = CastChecked<AWorldGeneratorInfo>(UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass()));

	this->Initialize();
	this->GenerateVoxels();
	this->GenerateProceduralMesh();
	this->ApplyProceduralMesh();

	return;
}

void ACommonChunk::Initialize(void)
{
	this->RawVoxels.SetNum(/* cubic */ AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::ChunkSize, false);
	this->JChunkPosition = this->GetActorLocation() * AWorldGeneratorInfo::UToJScale;
	this->ChunkKey = FIntVector(this->JChunkPosition / (AWorldGeneratorInfo::ChunkSize - 1));
}

void ACommonChunk::GenerateVoxels(void)
{
	switch (this->WorldGeneratorInfo->WorldGenerationType)
	{
	case EWorldGenerationType::Default:
	{
	}
	case EWorldGenerationType::SuperFlat:
	{
		this->GenerateSuperFlatWorld();
		return;
	}
	default:
	{
	}
	}

	UE_LOG(LogTemp, Error, TEXT("ACommonChunk::GenerateVoxels(): World Generation of type %s not implemented."), *WorldGeneratorInfo::LexToString(this->WorldGeneratorInfo->WorldGenerationType));

	return;
}

void ACommonChunk::ApplyProceduralMesh(void) const
{
	UMaterialSubsystem* MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();

	if (!MaterialSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("ACommonChunk::ApplyProceduralMesh(): Could not get UMaterialSubsystem."))
		return;
	}

	for (int i = 0; i < this->MeshData.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACommonChunk::ApplyProceduralMesh(): Applying procedural mesh data to procedural mesh component. Sweep: %d."), i)
		this->ProceduralMeshComponent->SetMaterial(i, MaterialSubsystem->MDynamicOpaque);

		this->ProceduralMeshComponent->CreateMeshSection(
			i,
			this->MeshData[i].Vertices,
			this->MeshData[i].Triangles,
			this->MeshData[i].Normals,
			this->MeshData[i].UV0,
			this->MeshData[i].Colors,
			this->MeshData[i].Tangents,
			true
		);
	}
}

void ACommonChunk::GenerateSuperFlatWorld()
{
	/*
	 * We of course should make this modular with some params that maybe even the user can provide.
	 */
    
	constexpr int StoneVoxel { 2 };
	constexpr int DirtVoxel  { 3 };
	constexpr int GrassVoxel { 4 };
    
	for (int X = 0; X < AWorldGeneratorInfo::ChunkSize; ++X)
	{
		for (int Y = 0; Y < AWorldGeneratorInfo::ChunkSize; ++Y)
		{
			for (int Z = 0; Z < AWorldGeneratorInfo::ChunkSize; ++Z)
			{
				const float WorldZ = this->JChunkPosition.Z + Z;

				if (WorldZ < 10)
				{
					this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = StoneVoxel;
					continue;
				}

				if (WorldZ < 11)
				{
					this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = DirtVoxel;
					continue;
				}

				if (WorldZ == 11)
				{
					this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = GrassVoxel;
					continue;
				}

				this->RawVoxels[ACommonChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = ECommonVoxels::Air;

				continue;
			}

			continue;
		}

		continue;
	}

	return;
}
