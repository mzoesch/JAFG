// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Cuboid.generated.h"


JAFG_VOID

class UVoxelSubsystem;
class UProceduralMeshComponent;

UCLASS(NotBlueprintable)
class JAFG_API ACuboid : public AActor
{
    GENERATED_BODY()

public:

    explicit ACuboid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    int32 CuboidX = 8;
    int32 CuboidY = 8;
    int32 CuboidZ = 8;

    int32 TexX = 10;
    int32 TexY = 10;
    int32 TexZ = 10;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProceduralMeshComponent* MeshComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    UPROPERTY()
    TObjectPtr<UMaterialSubsystem> MaterialSubsystem = nullptr;

public:

    void GenerateMesh(const voxel_t InAccumulated);

protected:

    voxel_t CurrentAccumulated = Accumulated::Null.AccumulatedIndex;

    int32 TriangleIndexCounter = 0;

    TArray<FVector>          Vertices;
    TArray<int32>            Triangles;
    TArray<FVector>          Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D>        UVs;
    TArray<FColor>           Colors;

    void ApplyMesh(void) const;
    /**
     * Adds a quadrilateral to the mesh for the next render sweep.
     *
     * @param V1      Top    Right Vertex
     * @param V2      Bottom Right Vertex
     * @param V3      Top    Left  Vertex
     * @param V4      Bottom Left  Vertex
     * @param Tangent Face Tangent
     */
    void CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent);
};
