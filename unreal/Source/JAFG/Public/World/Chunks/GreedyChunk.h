// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "World/Chunk.h"

#include "GreedyChunk.generated.h"

UCLASS()
class JAFG_API AGreedyChunk : public AChunk
{
    GENERATED_BODY()
    
public:

    AGreedyChunk();

private:
    
    struct FMask { int Voxel; int Normal; };

protected:

    virtual void GenerateMesh() override;

private:
    
    void CreateQuad(const FMask Mask, const FIntVector AxisMask, const int Width, const int Height, const FIntVector V1, const FIntVector V2, const FIntVector V3, const FIntVector V4);
    static bool CompareMask(const FMask M1, const FMask M2);
    
};
