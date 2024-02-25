// © 2023 mzoesch. All rights reserved.

#include "World/Chunk.h"

#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RandomStream.h"

#include "World/JCoordinate.h"
#include "Core/GI_Master.h"
#include "World/ChunkWorld.h"
#include "World/WorldVoxel.h"
#include "World/Voxels/Voxel.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GAME_INSTANCE                       Cast<UGI_Master>(this->GetGameInstance())
#define CW                                  this->ChunkWorld
#define WORLD_X(LocalX)                     (LocalX + this->ChunkPosition.X)
#define WORLD_Y(LocalY)                     (LocalY + this->ChunkPosition.Y)
#define WORLD_Z(LocalZ)                     (LocalZ + this->ChunkPosition.Z)

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
        UIL_LOG(Fatal, TEXT("Chunk Generation Failed!"))
        return;
    }
    this->GenerateMesh();
    this->ApplyMesh();

    return;
}

void AChunk::Setup()
{
    this->Voxels.SetNum(AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE * AChunk::CHUNK_SIZE, false);
    this->ChunkPosition = this->GetActorLocation() * UJCoordinate::U_TO_J_SCALE;
    this->ChunkKey = FIntVector((this->GetActorLocation() * UJCoordinate::U_TO_J_SCALE) / (AChunk::CHUNK_SIZE - 1));
    return;
}

void AChunk::GenerateVoxels()
{
    switch (CW->WorldGenerationType)
    {
    case EWorldGenerationType::WGT_SuperFlat:
        this->GenerateSuperFlatWorld();
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
        this->ProcMesh->SetMaterial(i, GAME_INSTANCE->MDynamicOpaque);
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

void AChunk::RegenerateMesh()
{
    this->ClearMesh();
    this->GenerateMesh();
    this->ApplyMesh();

    return;
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

                const float WorldZ = this->ChunkPosition.Z + Z;

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
                
                this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = EWorldVoxel::WV_Air;
                
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
    this->DWShapeTerrain();
    this->DWReplaceSurface();
    this->DWAddFeaturesAndStructures();

    return;
}

void AChunk::DWShapeTerrain()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; ++X)
    {
        const float WorldX = X + ChunkPosition.X;

        for (int Y = 0; Y < AChunk::CHUNK_SIZE; ++Y)
        {
            const float WorldY = Y + ChunkPosition.Y;

            const float NoiseValue = CW->NContinentalness->GetNoise(WorldX, WorldY);

            FNoiseSplinePointV2 LeftPoint;;
            FNoiseSplinePointV2 RightPoint;
            for (int i = 1; i < CW->ContinentalnessSplinePoints.Num(); ++i)
            {
                if (CW->ContinentalnessSplinePoints[i].NoiseValue >= NoiseValue)
                {
                    LeftPoint = CW->ContinentalnessSplinePoints[i - 1];
                    RightPoint = CW->ContinentalnessSplinePoints[i];
                    break;
                }
        
                continue;
            }
            const float Distance = (NoiseValue - LeftPoint.NoiseValue) / (RightPoint.NoiseValue - LeftPoint.NoiseValue);

            const float TargetPercentageHeight = ( ( (1 - Distance) * LeftPoint.TargetPercentageTerrainHeight ) + ( Distance * RightPoint.TargetPercentageTerrainHeight ) ) / 100.0f;

            const float HighestDensity = TargetPercentageHeight * 2.f - 1.f;
            
            
            for (int Z = AChunk::CHUNK_SIZE - 1; Z >= 0; --Z)
            {
                const float WorldZ = this->ChunkPosition.Z + Z;

                const float PercentageHeight = (WorldZ / CW->GetHighestPointV2()) * CW->FakeHeightMultiplier;
                const float Density = (HighestDensity + 1) * PercentageHeight - 1;

                this->Voxels[AChunk::GetVoxelIndex(FIntVector(X, Y, Z))] = CW->NWorld->GetNoise(WorldX, WorldY, WorldZ) < Density ? EWorldVoxel::WV_Air : 2;
                    
                continue;
            }
        
            continue;
        }
        
        continue;
    }

    return;
}

void AChunk::DWReplaceSurface()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; ++X)
    {
        for (int Y = 0; Y < AChunk::CHUNK_SIZE; ++Y)
        {
            constexpr int SurfaceBlock{4};
            constexpr int SubSurfaceBlock{3};
            constexpr int DefaultBlock{2};
            constexpr int MaxModified{3};

            int Modified = 0;

            const int TopVoxel = this->GetVoxel(FIntVector(X, Y, AChunk::CHUNK_SIZE));

            if (TopVoxel == DefaultBlock)
            {
                Modified = MaxModified;
            }

            else if (TopVoxel == SurfaceBlock)
            {
                Modified = 1;
            }

            /* This is kinda sketchy because now there may be parts with more subsurface blocks. */
            else if (TopVoxel == SubSurfaceBlock)
            {
                Modified = 2;
            }
            
            for (int Z = AChunk::CHUNK_SIZE - 1; Z >= 0; --Z)
            {
                if (this->GetVoxel(FIntVector(X, Y, Z)) == EWorldVoxel::WV_Air)
                {
                    Modified = 0;
                    continue;
                }

                if (Modified >= MaxModified)
                {
                    continue;
                }
                
                this->ModifyVoxelData(FIntVector(X, Y, Z), Modified == 0 ? SurfaceBlock : SubSurfaceBlock);
                Modified++;
                
                continue;
            }
    
            continue;
        }

        continue;
    }

    return;
}

void AChunk::AddTrees()
{
    /* We assume that the top voxel is grass. Or the surface block of the biome, when added. */

    constexpr int SurfaceBlock{4};

    bool bRegenerateTopChunk = false;
    
    for (int X = 0; X < AChunk::CHUNK_SIZE; ++X)
    {
        for (int Y = 0; Y < AChunk::CHUNK_SIZE; ++Y)
        {
            int TreeRootZ = -1;
            
            for (int Z = AChunk::CHUNK_SIZE - 1; Z >= 0; --Z)
            {
                if (this->GetVoxel(FIntVector(X, Y, Z)) == SurfaceBlock)
                {
                    TreeRootZ = Z + 1;
                    break;
                }

                continue;
            }

            /* We have to check if the chunk below has the surface block at the top of its border. */
            /* Currently does not work because we will always render the world from top to bottom. */
            if (TreeRootZ == -1)
            {
                if (CW->GetChunkByKey(this->GetBottomChunkKey()) == nullptr)
                {
                    continue;
                }

                if (CW->GetChunkByKey(this->GetBottomChunkKey())->GetVoxel(FIntVector(X, Y, AChunk::CHUNK_SIZE - 1)) != SurfaceBlock)
                {
                    continue;
                }

                TreeRootZ = 0;
            }
            
            if (TreeRootZ >= AChunk::CHUNK_SIZE)
            {
                continue;
            }

            /* TODO We also have to think of the height. On mountains are not trees ofc. */
            if (CW->NTree->GetNoise(WORLD_X(X), WORLD_Y(Y)) > CW->TreeThreshold)
            {
                continue;
            }

            constexpr int TreeHeight{5};
            
            /* We can currently do this because the top chunk is always created first. */
            for (int i = 0; i < TreeHeight; ++i)
            {
                this->ModifyVoxelCheckTopChunkOnly(FIntVector(X, Y, TreeRootZ + i), 5);
                continue;
            }

            /* Check if we have to re-generate the mesh of the above chunk */
            if (TreeRootZ + TreeHeight < AChunk::CHUNK_SIZE)
            {
                continue;
            }

            bRegenerateTopChunk = true;
            
            continue;
        }

        continue;
    }

    if (bRegenerateTopChunk)
    {
        CW->GetChunkByKey(this->GetTopChunkKey())->RegenerateMesh();
    }
    
    return;
}

void AChunk::DWAddFeaturesAndStructures()
{
    if (CW->bAddTrees)
    {
        this->AddTrees();
    }
    
    return;
}

AChunk* AChunk::GetTargetChunk(const FIntVector& LocalVoxelPosition, FIntVector& OutTransformedLocalVoxelPosition)
{
    if (LocalVoxelPosition.Z >= CHUNK_SIZE)
    {
        AChunk* TargetChunk = CW->GetChunkByKey(this->GetTopChunkKey());
        
        if (TargetChunk == nullptr)
        {
            return nullptr;
        }

        OutTransformedLocalVoxelPosition = FIntVector(LocalVoxelPosition.X, LocalVoxelPosition.Y, LocalVoxelPosition.Z - AChunk::CHUNK_SIZE);
    
        return TargetChunk;
    }
    
    if (LocalVoxelPosition.Z < 0)
    {
        AChunk* TargetChunk = CW->GetChunkByKey(this->GetBottomChunkKey()); 
    
        if (TargetChunk == nullptr)
        {
            return nullptr;
        }

        OutTransformedLocalVoxelPosition = FIntVector(LocalVoxelPosition.X, LocalVoxelPosition.Y, LocalVoxelPosition.Z + AChunk::CHUNK_SIZE);
        
        return TargetChunk;
    }

    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        return nullptr;
    }

    return this;
}

int AChunk::GetVoxel(const FIntVector& LocalVoxelPosition) const
{
    // TODO ...
    //      So we must check here if out of bounds for the neighboring
    //      chunk and get, if existing, the voxel data from there.

    if (LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE)
    {
        if (CW->GetChunkByKey(this->GetTopChunkKey()) == nullptr)
        {
            return EWorldVoxel::WV_Air;
        }

        return CW->GetChunkByKey(this->GetTopChunkKey())->GetVoxel(FIntVector(LocalVoxelPosition.X, LocalVoxelPosition.Y, 0));
    }
    
    if (
           LocalVoxelPosition.X >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Y >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.Z >= AChunk::CHUNK_SIZE
        || LocalVoxelPosition.X < 0
        || LocalVoxelPosition.Y < 0
        || LocalVoxelPosition.Z < 0
    )
    {
        return EWorldVoxel::WV_Air;
    }
    
    return this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)];
}

void AChunk::ModifyVoxel(const FIntVector& LocalVoxelPosition, const int Voxel)
{
    FIntVector TransformedLocalVoxelPosition = LocalVoxelPosition;
    AChunk* TargetChunk = this->GetTargetChunk(LocalVoxelPosition, TransformedLocalVoxelPosition);
    
    if (TargetChunk == nullptr)
    {
        UIL_LOG(Warning, TEXT("No associated chunk for local voxel position exists: %s."), *LocalVoxelPosition.ToString());
        return;
    }
    
    TargetChunk->ModifyVoxelData(TransformedLocalVoxelPosition, Voxel);
    TargetChunk->ClearMesh(); /* TODO Do we really have to regenerate the whole mesh? */
    TargetChunk->GenerateMesh();
    TargetChunk->ApplyMesh();

    return;
}

void AChunk::ModifyVoxelCheckTopChunkOnly(const FIntVector& CallingChunkLocalVoxelPosition, const int Voxel)
{
    if (CallingChunkLocalVoxelPosition.Z >= AChunk::CHUNK_SIZE)
    {
        AChunk* TargetChunk = CW->GetChunkByKey(this->GetTopChunkKey());
        
        if (TargetChunk == nullptr)
        {
            UIL_LOG(Warning, TEXT("No top chunk exists for local voxel position: %s."), *CallingChunkLocalVoxelPosition.ToString());
            return;
        }

        TargetChunk->ModifyVoxelData(FIntVector(CallingChunkLocalVoxelPosition.X, CallingChunkLocalVoxelPosition.Y, CallingChunkLocalVoxelPosition.Z - AChunk::CHUNK_SIZE), Voxel);

        return;
    }

    this->ModifyVoxelData(CallingChunkLocalVoxelPosition, Voxel);

    return;
}

//////////////////////////////////////////////////////////////////////////
// Non Generation Related
//////////////////////////////////////////////////////////////////////////

bool AChunk::HasCustomSecondaryCharacterEventVoxel(const FIntVector& LocalVoxelPosition, FVoxelMask& OutVoxelMask) const
{
    const int Voxel = this->Voxels[AChunk::GetVoxelIndex(LocalVoxelPosition)];
    
    if (Voxel == EWorldVoxel::WV_Null || Voxel == EWorldVoxel::WV_Air)
    {
        return false;
    }

    FVoxelMask Mask = GAME_INSTANCE->GetVoxelMask(FAccumulated(Voxel));

    UE_LOG(LogTemp, Warning, TEXT("AChunk::HasCustomSecondaryCharacterEventVoxel: Found %s."), *Mask.ToString());

    /* TODO JUST TEMP */
    if (Mask.VoxelClass == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AChunk::HasCustomSecondaryCharacterEventVoxel: VoxelClass is nullptr."));
        return false;
    }
    
    OutVoxelMask = Mask;
    
    return true;
}

#undef UIL_LOG
#undef GAME_INSTANCE
#undef CW
#undef WORLD_X
#undef WORLD_Y
#undef WORLD_Z
