// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "World/Voxel.h"

#include "Cuboid.generated.h"

class UProceduralMeshComponent;

UCLASS()
class ACuboid : public AActor
{
    GENERATED_BODY()

public:

    ACuboid();
    
public:

    /* All measurements in Unreal Coordinate System. */

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 CuboidX = 100;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 CuboidY = 100;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 CuboidZ = 100;

protected:

    UPROPERTY(VisibleAnywhere)
    UProceduralMeshComponent* Mesh;

public:

    void GenerateMesh(const EVoxel VToGenerate);
    /* TODO Generate Mesh for EItem. */

private:

    // TODO
    //      Do we have to calculate Tangents? See the Greedy Meshing Algorithm.
    //      Works without this? What are the drawbacks and benefits?

    EVoxel Voxel;
    
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D> UVs;
    TArray<FColor> Colors;

    void GenerateMesh();
    void CreateQuadrilateral(const FVector& TopRight, const FVector& BottomRight, const FVector& TopLeft, const FVector& BottomLeft, const FProcMeshTangent& Tangent);
    void ApplyMesh() const;

    int32 TriangleIndexCounter = 0;
    FVector PreDefinedShape[8];

    int GetTextureIndex(const EVoxel VToGet, const FVector& Normal) const;
    
};
