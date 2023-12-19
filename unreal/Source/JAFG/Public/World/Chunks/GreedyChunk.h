// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "World/Chunk.h"
#include "World/Voxel.h"

#include "GreedyChunk.generated.h"

UCLASS()
class JAFG_API AGreedyChunk : public AChunk
{
    GENERATED_BODY()
    
public:

    AGreedyChunk();

private:
    
    struct FMask { EVoxel Block; int Normal; };

protected:

    virtual void Setup() override;
    virtual void InitiateVoxels() override;
    virtual void GenerateMesh() override;
    virtual void ModifyVoxelData(const FIntVector& LocalVoxelPosition, const EVoxel Voxel) override;

private:

    /* TODO Move to super class? */
    FVector ActorCoordinate;
    
    TArray<EVoxel> Voxels;

private:

    void GenerateDevVoxel(const FIntVector& LocalVoxelPosition, const int VoxelPillarHeight);

    void CreateQuad(const FMask Mask, const FIntVector AxisMask, const int Width, const int Height, const FIntVector V1, const FIntVector V2, const FIntVector V3, const FIntVector V4);
    static bool CompareMask(const FMask M1, const FMask M2);
    static int GetTextureIndex(const EVoxel Voxel, const FVector& Normal);
    
    EVoxel GetVoxel(const FIntVector& LocalVoxelPosition) const;
    static int GetVoxelIndex(const FIntVector& LocalVoxelPosition);
    
};
