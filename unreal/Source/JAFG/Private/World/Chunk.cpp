// © 2023 mzoesch. All rights reserved.

#include "World/Chunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "World/JCoordinate.h"
#include "Core/GI_Master.h"
#include "World/ChunkWorld.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GI CastChecked<UGI_Master>(this->GetGameInstance())
#define CW this->ChunkWorld

AChunk::AChunk()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
    this->ProcMesh->SetCastShadow(true);
    this->SetRootComponent(this->ProcMesh);

    this->bGenerationFailed = false;
    
    return;
}

void AChunk::BeginPlay()
{
    Super::BeginPlay();

    this->ChunkWorld = CastChecked<AChunkWorld>(UGameplayStatics::GetActorOfClass(this, AChunkWorld::StaticClass()));

    this->bGenerationFailed = false;
    
    this->Setup();
    this->GenerateVoxels();
    if (this->bGenerationFailed)
    {
        UE_LOG(LogTemp, Error, TEXT("Chunk Generation Failed!"))
        return;
    }
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}

void AChunk::Setup()
{
    this->Voxels.SetNum(AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE, false);
    this->ActorCoordinate = this->GetActorLocation() * AJCoordinate::U_TO_J_SCALE;
    return;
}

void AChunk::GenerateVoxels()
{
    switch (CW->WorldGenerationType)
    {
    case EWorldGenerationType::WGT_SuperFlat:
        this->GenerateSuperFlatWorld();
        break;
    case EWorldGenerationType::WGT_Legacy:
        this->GenerateLegacyWorld();
        break;
    case EWorldGenerationType::WGT_Pure3DDefault:
        this->GeneratePure3DDefaultWorld();
        break;
    case EWorldGenerationType::WGT_Default:
        this->GenerateDefaultWorld();
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown World Generation Type!"))
        this->bGenerationFailed = true;
        break;
    }

    return;
}

void AChunk::ApplyMesh() const
{
    for (int i = 0; i < this->MeshDataArray.Num(); i++)
    {
        this->ProcMesh->SetMaterial(i, GI->MDynamicOpaque);
        this->ProcMesh->CreateMeshSection(
            i,
            this->MeshDataArray[i].Vertices,
            this->MeshDataArray[i].Triangles,
            this->MeshDataArray[i].Normals,
            this->MeshDataArray[i].UV0,
            this->MeshDataArray[i].Colors,
            this->MeshDataArray[i].Tangents,
            true
        );

        continue;
    }

    return;
}

void AChunk::ClearMesh()
{
    // TODO What is faster?
    // this->MeshDataArray.Empty();
    // this->VertexCounts.Empty();

    for (int i = 0; i < this->MeshDataArray.Num(); i++)
    {
        this->MeshDataArray[i].Clear();
        continue;
    }

    for (int i = 0; i < this->VertexCounts.Num(); i++)
    {
        this->VertexCounts[i] = 0;
        continue;
    }
    
    return;
}

void AChunk::ModifyVoxelData(const FIntVector& LocalVoxelPosition, const int Voxel)
{
    this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = Voxel;
}

int AChunk::GetVoxelIndex(const FIntVector& LocalVoxelPosition)
{
    return LocalVoxelPosition.Z * AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE + LocalVoxelPosition.Y * AChunk::CHUNK_SIZE + LocalVoxelPosition.X;
}

void AChunk::GenerateSuperFlatWorld()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; X++)
    {
        for (int Y = 0; Y < AChunk::CHUNK_SIZE; Y++)
        {
            for (int Z = 0; Z < AChunk::CHUNK_SIZE; Z++)
            {
                /* TODO We might want to move this to a JSON. And parse some params to this method to make it modular. */

                const float WorldZ = this->ActorCoordinate.Z + Z;

                if (WorldZ < 10)
                {
                    this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = 2; /* Stone */
                    continue;
                }

                if (WorldZ < 11)
                {
                    this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = 3; /* Dirt */
                    continue;
                }

                if (WorldZ == 11)
                {
                    this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = 4; /* Grass */
                    continue;
                }
                
                this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = EWorldVoxel::AirVoxel;
                
                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void AChunk::GenerateLegacyWorld()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; X++)
    {
        for (int Y = 0; Y < AChunk::CHUNK_SIZE; Y++)
        {
            const float WorldX = X + ActorCoordinate.X;
            const float WorldY = Y + ActorCoordinate.Y;
            const int VoxelPillarHeight = FMath::RoundToInt((this->ChunkWorld->NContinentalness->GetNoise(WorldX, WorldY) + 1) * AChunk::CHUNK_SIZE / 2);
    
            for (int Z = 0; Z < AChunk::CHUNK_SIZE; Z++)
            {
                const FIntVector LocalVoxelPosition = FIntVector(X, Y, Z);
                
                const int WorldZ = this->ActorCoordinate.Z + LocalVoxelPosition.Z;

                if (WorldZ < VoxelPillarHeight - 3)
                {
                    this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = 2; /* Stone */
                    continue;
                }

                if (WorldZ < VoxelPillarHeight - 1)
                {
                    this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = 3; /* Dirt */
                    continue;
                }

                if (WorldZ == VoxelPillarHeight - 1)
                {
                    this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = 4; /* Grass */
                    continue;
                }

                this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)] = EWorldVoxel::AirVoxel;
                
                continue;
            }
    
            continue;
        }
    
        continue;
    }

    return;
}

void AChunk::GeneratePure3DDefaultWorld()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; ++X)
    {
        const float WorldX = X + ActorCoordinate.X;

        for (int Y = 0; Y < AChunk::CHUNK_SIZE; ++Y)
        {
            const float WorldY = Y + ActorCoordinate.Y;
    
            for (int Z = 0; Z < AChunk::CHUNK_SIZE; ++Z)
            {
                const float WorldZ = this->ActorCoordinate.Z + Z;
                
                const float NoiseValue = CW->NContinentalness->GetNoise(WorldX, WorldY, WorldZ);
                const float Density = CW->GetDensity(WorldX, WorldY, WorldZ);
                
                this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = NoiseValue > Density ? 2 : EWorldVoxel::AirVoxel;
                
                continue;
            }
    
            continue;
        }
    
        continue;
    }

    return;
}

void AChunk::GenerateDefaultWorld()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; ++X)
    {
        const float WorldX = X + ActorCoordinate.X;

        for (int Y = 0; Y < AChunk::CHUNK_SIZE; ++Y)
        {
            const float WorldY = Y + ActorCoordinate.Y;

            const float NoisePillarHeight = CW->NContinentalness->GetNoise(WorldX, WorldY);
            const float FillUpTo = (NoisePillarHeight + 1.0f) / 2.0f * CW->GetWorldHeight();

            
            for (int Z = 0; Z < AChunk::CHUNK_SIZE; ++Z)
            {
                const float Density = CW->GetDensity(WorldX, WorldY, this->ActorCoordinate.Z + Z);
                
                this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = Density < FillUpTo ? 2 : EWorldVoxel::AirVoxel;
                    
                continue;
            }
        
            continue;
        }
        
        continue;
    }

    return;
}

int AChunk::GetVoxel(const FIntVector& LocalVoxelPosition) const
{
    // TODO
    //      So we must check here if out of bounds for the neighboring
    //      chunk and get, if existing, the voxel data from there.
    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        return EWorldVoxel::AirVoxel;
    }
    
    return this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)];
}

void AChunk::ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel)
{
    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        UE_LOG(LogTemp, Warning, TEXT("Voxel out of bounds! Not implemented yet! %s"), *LocalVoxelPosition.ToString());
        return;
    }
    
    this->ModifyVoxelData(LocalVoxelPosition, Voxel);
    this->ClearMesh(); /* TODO Do we really have to regenerate the whole mesh? */
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}

#undef UIL_LOG
#undef GI
#undef CW
