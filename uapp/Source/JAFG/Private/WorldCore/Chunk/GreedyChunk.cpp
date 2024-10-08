// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/GreedyChunk.h"
#include "System/VoxelSubsystem.h"

AGreedyChunk::AGreedyChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
    this->bReplicates = false;

    return;
}

void AGreedyChunk::GenerateProceduralMesh(void)
{
    Super::GenerateProceduralMesh();

    /* Sweep over each axis XYZ. */
    for (int Axis = 0; Axis < 3; ++Axis)
    {
        /* 2 Perpendicular axis. */
        const int Axis1 = (Axis + 1) % 3;
        const int Axis2 = (Axis + 2) % 3;

        constexpr int MainAxisLimit = WorldStatics::ChunkSize;
        constexpr int Axis1Limit    = WorldStatics::ChunkSize;
        constexpr int Axis2Limit    = WorldStatics::ChunkSize;

        auto DeltaAxis1 = FIntVector::ZeroValue;
        auto DeltaAxis2 = FIntVector::ZeroValue;

        auto ChunkItr = FIntVector::ZeroValue;
        auto AxisMask = FIntVector::ZeroValue;

        AxisMask[Axis] = 1;

        TArray<FMask> Mask;
        Mask.SetNum(Axis1Limit * Axis2Limit, false);

        /* Check each slice of the chunk. */
        for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
        {
            int N = 0;

            /* Compute mask. */
            for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
            {
                for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
                {
                    const voxel_t CurrentBlock = this->GetFastLocalAndAdjacentVoxel(ChunkItr);
                    const voxel_t CompareBlock = this->GetFastLocalAndAdjacentVoxel(ChunkItr + AxisMask);

                    const bool bCurrentBlockOpaque = CurrentBlock > ECommonVoxels::Max;
                    const bool bCompareBlockOpaque = CompareBlock > ECommonVoxels::Max;

                    if (bCurrentBlockOpaque == bCompareBlockOpaque)
                    {
                        Mask[N++] = FMask{ECommonVoxels::Null, 0};
                    }
                    else if (bCurrentBlockOpaque)
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
            /* Generate Mesh From Mask. */
            for (int j = 0; j < Axis2Limit; ++j)
            {
                for (int i = 0; i < Axis1Limit;)
                {
                    if (Mask[N].Normal == 0)
                    {
                        ++i;
                        ++N;

                        continue;
                    }

                    const FMask CurrentMask = Mask[N];
                    ChunkItr[Axis1] = i;
                    ChunkItr[Axis2] = j;

                    int Width;

                    for (Width = 1; i + Width < Axis1Limit && AGreedyChunk::Equals(Mask[N + Width], CurrentMask); ++Width)
                    {
                    }

                    int  Height;
                    bool Done = false;

                    for (Height = 1; j + Height < Axis2Limit; ++Height)
                    {
                        for (int k = 0; k < Width; ++k)
                        {
                            if (AGreedyChunk::Equals(Mask[N + k + Height * Axis1Limit], CurrentMask))
                            {
                                continue;
                            }

                            Done = true;
                            break;
                        }

                        if (Done)
                        {
                            break;
                        }

                        continue;
                    }

                    DeltaAxis1[Axis1] = Width;
                    DeltaAxis2[Axis2] = Height;

                    this->CreateQuadrilateral(
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
                            Mask[N + k + l * Axis1Limit] = FMask{ECommonVoxels::Null, 0};
                        }
                    }

                    i += Width;
                    N += Width;

                    continue;
                }

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}

void AGreedyChunk::CreateQuadrilateral(
    const FMask& Mask, const FIntVector& AxisMask, const int Width, const int Height,
    const FIntVector& V1, const FIntVector& V2, const FIntVector& V3, const FIntVector& V4
)
{
    const FVector Normal = FVector(AxisMask * Mask.Normal);
    const FColor Color = FColor(0, 96, 0, this->GetVoxelSubsystem()->GetTextureIndex(Mask.Voxel, Normal));
    const int TextureGroup = this->GetVoxelSubsystem()->GetVoxelTextureGroup(Mask.Voxel, Normal);

    if (TextureGroup > this->VertexCounts.Num() - 1)
    {
        const int Delta = TextureGroup - this->VertexCounts.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->VertexCounts.Add(0);
            continue;
        }
    }

    if (TextureGroup > this->MeshData.Num() - 1)
    {
        const int Delta = TextureGroup - this->MeshData.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->MeshData.Add(FChunkMeshData());
            continue;
        }
    }

    /* TODO Should we calculate tangents for mesh here? Works without it but what are the drawbacks. */
    /* UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, MeshData.UV0, MeshData.Normals, MeshData.Tangents); */

    this->MeshData[TextureGroup].Vertices.Append({
        FVector(V1) * 100, FVector(V2) * 100, FVector(V3) * 100, FVector(V4) * 100
    });
    this->MeshData[TextureGroup].Triangles.Append({
        this->VertexCounts[TextureGroup],
        this->VertexCounts[TextureGroup] + 2 + Mask.Normal,
        this->VertexCounts[TextureGroup] + 2 - Mask.Normal,
        this->VertexCounts[TextureGroup] + 3,
        this->VertexCounts[TextureGroup] + 1 - Mask.Normal,
        this->VertexCounts[TextureGroup] + 1 + Mask.Normal
    });
    this->MeshData[TextureGroup].Normals.Append({Normal, Normal, Normal, Normal});
    this->MeshData[TextureGroup].Colors.Append({Color, Color, Color, Color});

    if (Normal.X == 1 || Normal.X == -1)
    {
        this->MeshData[TextureGroup].UV0.Append({
            FVector2D(Width, Height),
            FVector2D(0, Height),
            FVector2D(Width, 0),
            FVector2D(0, 0),
        });
    }
    else
    {
        this->MeshData[TextureGroup].UV0.Append({
            FVector2D(Height, Width),
            FVector2D(Height, 0),
            FVector2D(0, Width),
            FVector2D(0, 0),
        });
    }

    this->VertexCounts[TextureGroup] += 4;

    return;
}
