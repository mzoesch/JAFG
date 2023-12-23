// © 2023 mzoesch. All rights reserved.

#include "World/Chunks/GreedyChunk.h"

#include "Lib/FastNoiseLite.h"
#include "World/JCoordinate.h"

AGreedyChunk::AGreedyChunk()
{
    this->PrimaryActorTick.bCanEverTick = false;
    return;
}

void AGreedyChunk::Setup()
{
    this->ActorCoordinate = this->GetActorLocation() * AJCoordinate::U_TO_J_SCALE;
    return;
}

void AGreedyChunk::InitiateVoxels()
{
    for (int X = 0; X < AChunk::CHUNK_SIZE; X++)
    {
        for (int Y = 0; Y < AChunk::CHUNK_SIZE; Y++)
        {
            const float WorldX = X + ActorCoordinate.X;
            const float WorldY = Y + ActorCoordinate.Y;
            const int VoxelPillarHeight = FMath::RoundToInt((Noise->GetNoise(WorldX, WorldY) + 1) * AChunk::CHUNK_SIZE / 2);

            for (int Z = 0; Z < AChunk::CHUNK_SIZE; Z++)
            {
                this->GenerateDevVoxel(FIntVector(X, Y, Z), VoxelPillarHeight);
                continue;
            }


            continue;
        }

        continue;
    }

    return;
}

void AGreedyChunk::GenerateDevVoxel(const FIntVector& LocalVoxelPosition, const int VoxelPillarHeight)
{
    const int WorldZ = this->ActorCoordinate.Z + LocalVoxelPosition.Z;

    if (WorldZ < VoxelPillarHeight - 3)
    {
        this->Voxels[AGreedyChunk::GetVoxelIndex(LocalVoxelPosition)] = EVoxel::Stone;
        return;
    }

    if (WorldZ < VoxelPillarHeight - 1)
    {
        this->Voxels[AGreedyChunk::GetVoxelIndex(LocalVoxelPosition)] = EVoxel::Dirt;
        return;
    }

    if (WorldZ == VoxelPillarHeight - 1)
    {
        this->Voxels[AGreedyChunk::GetVoxelIndex(LocalVoxelPosition)] = EVoxel::Grass;
        return;
    }

    this->Voxels[AGreedyChunk::GetVoxelIndex(LocalVoxelPosition)] = EVoxel::Air;
    
    return;
}

void AGreedyChunk::CreateQuad(const FMask Mask, const FIntVector AxisMask, const int Width, const int Height, const FIntVector V1, const FIntVector V2, const FIntVector V3, const FIntVector V4)
{
    const FVector Normal = FVector(AxisMask * Mask.Normal);

    const FColor Color = Mask.Block == EVoxel::Glass ? FColor(0, 0, 0, 0) : FColor(0, 0, 0, this->GetTextureIndex(Mask.Block, Normal));
    
    // HERE Calculate Tangents for Mesh

#pragma region Translucent

    if (Mask.Block == EVoxel::Glass || Mask.Block == EVoxel::Leaves)
    {
        this->TranslucentMeshData.Vertices.Append({
            FVector(V1) * 100,
            FVector(V2) * 100,
            FVector(V3) * 100,
            FVector(V4) * 100
        });

        this->TranslucentMeshData.Triangles.Append({
            this->TranslucentVertexCount,
            this->TranslucentVertexCount + 2 + Mask.Normal,
            this->TranslucentVertexCount + 2 - Mask.Normal,
            this->TranslucentVertexCount + 3,
            this->TranslucentVertexCount + 1 - Mask.Normal,
            this->TranslucentVertexCount + 1 + Mask.Normal
        }); 

        this->TranslucentMeshData.Normals.Append({
            Normal,
            Normal,
            Normal,
            Normal
        });

        this->TranslucentMeshData.Colors.Append({
            Color,
            Color,
            Color,
            Color
        });

        if (Normal.X == 1 || Normal.X == -1)
        {
            this->TranslucentMeshData.UV0.Append({
                FVector2D(Width, Height),
                FVector2D(0, Height),
                FVector2D(Width, 0),
                FVector2D(0, 0),
            });
        }
        else
        {
            this->TranslucentMeshData.UV0.Append({
                FVector2D(Height, Width),
                FVector2D(Height, 0),
                FVector2D(0, Width),
                FVector2D(0, 0),
            });
        }

        this->TranslucentVertexCount += 4;
        
        return;
    }
    
#pragma endregion Translucent
    
    this->MeshData.Vertices.Append({
        FVector(V1) * 100,
        FVector(V2) * 100,
        FVector(V3) * 100,
        FVector(V4) * 100
    });

    this->MeshData.Triangles.Append({
        this->VertexCount,
        this->VertexCount + 2 + Mask.Normal,
        this->VertexCount + 2 - Mask.Normal,
        this->VertexCount + 3,
        this->VertexCount + 1 - Mask.Normal,
        this->VertexCount + 1 + Mask.Normal
    });

    this->MeshData.Normals.Append({
        Normal,
        Normal,
        Normal,
        Normal
    });

    this->MeshData.Colors.Append({
        Color,
        Color,
        Color,
        Color
    });

    if (Normal.X == 1 || Normal.X == -1)
    {
        this->MeshData.UV0.Append({
            FVector2D(Width, Height),
            FVector2D(0, Height),
            FVector2D(Width, 0),
            FVector2D(0, 0),
        });
    }
    else
    {
        this->MeshData.UV0.Append({
            FVector2D(Height, Width),
            FVector2D(Height, 0),
            FVector2D(0, Width),
            FVector2D(0, 0),
        });
    }

    this->VertexCount += 4;

    // UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, MeshData.UV0, MeshData.Normals, MeshData.Tangents);

    return;
}

bool AGreedyChunk::CompareMask(const FMask M1, const FMask M2)
{
    return M1.Block == M2.Block && M1.Normal == M2.Normal;
}

int AGreedyChunk::GetTextureIndex(const EVoxel Voxel, const FVector& Normal)
{
    switch (Voxel)
    {
    case EVoxel::Stone: return 0;
    case EVoxel::Dirt: return 1;
    case EVoxel::Grass:
    {
        if (Normal == FVector::DownVector)
        {
            return 1;
        }
            
        if (Normal == FVector::UpVector)
        {
            return 2;
        }

        return 3;
    }
    case EVoxel::Glass: return 0;
    case EVoxel::Log:
    {
        if (Normal == FVector::UpVector || Normal == FVector::DownVector)
        {
            return 5;
        }

        return 4;
    }
    case EVoxel::Planks: return 6;
    case EVoxel::Leaves: return 1;
    default: return 255;
    }
}

void AGreedyChunk::GenerateMesh()
{
    // Sweep over each axis (X, Y, Z)
    for (int Axis = 0; Axis < 3; ++Axis)
    {
        // 2 Perpendicular axis
        const int Axis1 = (Axis + 1) % 3;
        const int Axis2 = (Axis + 2) % 3;

        const int MainAxisLimit = AChunk::CHUNK_SIZE;
        const int Axis1Limit = AChunk::CHUNK_SIZE;
        const int Axis2Limit = AChunk::CHUNK_SIZE;

        auto DeltaAxis1 = FIntVector::ZeroValue;
        auto DeltaAxis2 = FIntVector::ZeroValue;

        auto ChunkItr = FIntVector::ZeroValue;
        auto AxisMask = FIntVector::ZeroValue;

        AxisMask[Axis] = 1;

        TArray<FMask> Mask;
        Mask.SetNum(Axis1Limit * Axis2Limit);

        // Check each slice of the chunk
        for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
        {
            int N = 0;

            // Compute Mask
            for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
            {
                for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
                {
                    const auto CurrentBlock = this->GetVoxel(ChunkItr);
                    const auto CompareBlock = this->GetVoxel(ChunkItr + AxisMask);

                    const bool CurrentBlockOpaque = CurrentBlock != EVoxel::Air;
                    const bool CompareBlockOpaque = CompareBlock != EVoxel::Air;

                    if (CurrentBlockOpaque == CompareBlockOpaque)
                    {
                        if (CompareBlock == EVoxel::Glass)
                        {
                            if (CompareBlock == EVoxel::Glass && CurrentBlock == EVoxel::Glass)
                            {
                                Mask[N++] = FMask{EVoxel::Null, 0};
                            }
                            else
                            {
                                Mask[N++] = FMask{CurrentBlock, 1};
                            }
                        }
                        else if (CompareBlock == EVoxel::Leaves)
                        {
                            Mask[N++] = FMask{CurrentBlock, 1};
                        }
                        else
                        {
                            Mask[N++] = FMask{EVoxel::Null, 0};
                        }
                    }
                    else if (CurrentBlockOpaque)
                    {
                        Mask[N++] = FMask{CurrentBlock, 1};
                    }
                    else
                    {
                        Mask[N++] = FMask{CompareBlock, -1};
                    }
                }
            }

            ++ChunkItr[Axis];
            N = 0;

            // Generate Mesh From Mask
            for (int j = 0; j < Axis2Limit; ++j)
            {
                for (int i = 0; i < Axis1Limit;)
                {
                    if (Mask[N].Normal != 0)
                    {
                        const auto CurrentMask = Mask[N];
                        ChunkItr[Axis1] = i;
                        ChunkItr[Axis2] = j;

                        int Width;

                        for (Width = 1; i + Width < Axis1Limit && AGreedyChunk::CompareMask(Mask[N + Width], CurrentMask); ++Width)
                        {
                        }

                        int Height;
                        bool Done = false;

                        for (Height = 1; j + Height < Axis2Limit; ++Height)
                        {
                            for (int k = 0; k < Width; ++k)
                            {
                                if (CompareMask(Mask[N + k + Height * Axis1Limit], CurrentMask)) continue;

                                Done = true;
                                break;
                            }

                            if (Done) break;
                        }

                        DeltaAxis1[Axis1] = Width;
                        DeltaAxis2[Axis2] = Height;

                        CreateQuad(
                            CurrentMask, AxisMask, Width, Height,
                            ChunkItr,
                            ChunkItr + DeltaAxis1,
                            ChunkItr + DeltaAxis2,
                            ChunkItr + DeltaAxis1 + DeltaAxis2
                        );

                        DeltaAxis1 = FIntVector::ZeroValue;
                        DeltaAxis2 = FIntVector::ZeroValue;

                        for (int l = 0; l < Height; ++l)
                        {
                            for (int k = 0; k < Width; ++k)
                            {
                                Mask[N + k + l * Axis1Limit] = FMask{EVoxel::Null, 0};
                            }
                        }

                        i += Width;
                        N += Width;
                    }
                    else
                    {
                        i++;
                        N++;
                    }
                }
            }
        }
    }
    
    return;
}

void AGreedyChunk::ModifyVoxelData(const FIntVector& LocalVoxelPosition, const EVoxel Voxel)
{
    this->Voxels[AGreedyChunk::GetVoxelIndex(LocalVoxelPosition)] = Voxel;
    return;
}
