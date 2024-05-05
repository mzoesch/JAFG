// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ProceduralMeshComponent.h"

JAFG_VOID

struct JAFG_API FChunkMeshData
{
    TArray<FVector>   Vertices;
    TArray<int32>     Triangles;
    TArray<FVector>   Normals;
    TArray<FColor>    Colors;
    TArray<FVector2D> UV0;
    /**
     * Do we need tangent calculations? They are very expensive to calculate but are not required currently.
     * We may need this later for lighting calculations? To be determined.
     */
    TArray<FProcMeshTangent> Tangents;

    FORCEINLINE auto Clear(void) -> void
    {
        this->Vertices.Reset(this->Vertices.Num());
        this->Triangles.Reset(this->Triangles.Num());
        this->Normals.Reset(this->Normals.Num());
        this->Colors.Reset(this->Colors.Num());
        this->UV0.Reset(this->UV0.Num());
        this->Tangents.Reset(this->Tangents.Num());

        return;
    }
};
