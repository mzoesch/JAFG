// © 2023 mzoesch. All rights reserved.

#include "World/Chunks/GreedyChunk.h"

#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"

#define GI CastChecked<UGI_Master>(this->GetGameInstance())

AGreedyChunk::AGreedyChunk()
{
    this->PrimaryActorTick.bCanEverTick = false;
    this->VertexCounts = TArray<int>();
    return;
}

void AGreedyChunk::CreateQuad(const FMask Mask, const FIntVector AxisMask, const int Width, const int Height, const FIntVector V1, const FIntVector V2, const FIntVector V3, const FIntVector V4)
{
    const FVector   Normal          = FVector(AxisMask * Mask.Normal);
    const FColor    Color           = FColor(0, 128, 0, GI->GetTextureIndex(Mask.Voxel, Normal));
    const int       TextureGroup    = GI->GetTextureGroup(Mask.Voxel, Normal);

    if (TextureGroup > this->VertexCounts.Num() - 1)
    {
        const int Delta = TextureGroup - this->VertexCounts.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->VertexCounts.Add(0);
            continue;
        }
    }

    if (TextureGroup > this->MeshDataArray.Num() - 1)
    {
        const int Delta = TextureGroup - this->MeshDataArray.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->MeshDataArray.Add(FChunkMeshData());
            continue;
        }
    }
    
    /* TODO Should we calculate tangents for mesh here? Works without it but what are the drawbacks. */
    /* UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, MeshData.UV0, MeshData.Normals, MeshData.Tangents); */
    
    this->MeshDataArray[TextureGroup].Vertices.Append({FVector(V1) * 100, FVector(V2) * 100, FVector(V3) * 100, FVector(V4) * 100});
    this->MeshDataArray[TextureGroup].Triangles.Append({
        this->VertexCounts[TextureGroup],
        this->VertexCounts[TextureGroup] + 2 + Mask.Normal,
        this->VertexCounts[TextureGroup] + 2 - Mask.Normal,
        this->VertexCounts[TextureGroup] + 3,
        this->VertexCounts[TextureGroup] + 1 - Mask.Normal,
        this->VertexCounts[TextureGroup] + 1 + Mask.Normal
    });
    this->MeshDataArray[TextureGroup].Normals.Append({Normal, Normal, Normal, Normal});
    this->MeshDataArray[TextureGroup].Colors.Append({Color, Color, Color, Color});

    if (Normal.X == 1 || Normal.X == -1)
    {
        this->MeshDataArray[TextureGroup].UV0.Append({
            FVector2D(Width, Height),
            FVector2D(0, Height),
            FVector2D(Width, 0),
            FVector2D(0, 0),
        });
    }
    else
    {
        this->MeshDataArray[TextureGroup].UV0.Append({
            FVector2D(Height, Width),
            FVector2D(Height, 0),
            FVector2D(0, Width),
            FVector2D(0, 0),
        });
    }

    this->VertexCounts[TextureGroup] += 4;

    return;
}

bool AGreedyChunk::CompareMask(const FMask M1, const FMask M2)
{
    return M1.Voxel == M2.Voxel && M1.Normal == M2.Normal;
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

                    const bool CurrentBlockOpaque = CurrentBlock != EWorldVoxel::WV_Air;
                    const bool CompareBlockOpaque = CompareBlock != EWorldVoxel::WV_Air;

                    if (CurrentBlockOpaque == CompareBlockOpaque)
                    {
                        Mask[N++] = FMask{EWorldVoxel::WV_Null, 0};
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
                                Mask[N + k + l * Axis1Limit] = FMask{EWorldVoxel::WV_Null, 0};
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

#undef GI
