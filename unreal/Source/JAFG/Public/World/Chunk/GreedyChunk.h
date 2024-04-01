// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonChunk.h"

#include "GreedyChunk.generated.h"

class UVoxelSubsystem;

UCLASS(NotBlueprintable)
class JAFG_API AGreedyChunk : public ACommonChunk
{
    GENERATED_BODY()

public:
    
    explicit AGreedyChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
    
    /* TODO Can we change the voxel? */
    struct FMask
    {
        int Voxel;
        int Normal;
    };

    FORCEINLINE static bool CompareMask(const FMask& A, const FMask& B)
    {
        return A.Voxel == B.Voxel && A.Normal == B.Normal;
    };

protected:
    
    virtual void GenerateProceduralMesh(void) override;

private:
    
    void CreateQuadrilateral(
        const FMask& Mask, const FIntVector& AxisMask, const int Width, const int Height,
        const FIntVector& V1, const FIntVector& V2, const FIntVector& V3, const FIntVector& V4
    );
};
