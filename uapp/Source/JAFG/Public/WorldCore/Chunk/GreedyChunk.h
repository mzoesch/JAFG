// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonChunk.h"

#include "GreedyChunk.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AGreedyChunk : public ACommonChunk
{
    GENERATED_BODY()

public:

    explicit AGreedyChunk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void GenerateProceduralMesh(void) override;

private:

    struct FMask
    {
        voxel_t Voxel;
        int32   Normal;
    };

    FORCEINLINE static bool Equals(const FMask& A, const FMask& B)
    {
        return A.Voxel == B.Voxel && A.Normal == B.Normal;
    }

    void CreateQuadrilateral(
        const FMask& Mask, const FIntVector& AxisMask, const int Width, const int Height,
        const FIntVector& V1, const FIntVector& V2, const FIntVector& V3, const FIntVector& V4
    );
};
