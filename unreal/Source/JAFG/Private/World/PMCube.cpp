// © 2023 mzoesch. All rights reserved.

#include "World/PMCube.h"

#include "ProceduralMeshComponent.h"

APMCube::APMCube()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
    this->RootComponent = this->SceneComponent;

    this->RootComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->ProceduralMeshComponent = Cast<UProceduralMeshComponent>(this->RootComponent);
    this->ProceduralMeshComponent->SetupAttachment(this->SceneComponent);
    
    return;
}

void APMCube::BeginPlay()
{
    Super::BeginPlay();

    this->GenerateMesh();
    
    return;
}

void APMCube::GenerateMesh()
{
    this->Vertices.Reset();
    this->Triangles.Reset();
    this->Normals.Reset();
    this->Tangents.Reset();
    this->UVs.Reset();
    this->VertexColors.Reset();

    int32 TriangleIndexCount = 0;
    FVector DefinedShape[8];
    // ReSharper disable once CppJoinDeclarationAndAssignment
    FProcMeshTangent TangentSetup;

    DefinedShape[0] = FVector(this->VoxelRadius.X, this->VoxelRadius.Y, this->VoxelRadius.Z); /* Forward Top Right */
    DefinedShape[1] = FVector(this->VoxelRadius.X, this->VoxelRadius.Y, -this->VoxelRadius.Z); /* Forward Bottom Right */
    DefinedShape[2] = FVector(this->VoxelRadius.X, -this->VoxelRadius.Y, this->VoxelRadius.Z); /* Forward Top Left */
    DefinedShape[3] = FVector(this->VoxelRadius.X, -this->VoxelRadius.Y, -this->VoxelRadius.Z); /* Forward Bottom Left */
    DefinedShape[4] = FVector(-this->VoxelRadius.X, -this->VoxelRadius.Y, this->VoxelRadius.Z); /* Backward Top Left */
    DefinedShape[5] = FVector(-this->VoxelRadius.X, -this->VoxelRadius.Y, -this->VoxelRadius.Z); /* Backward Bottom Left */
    DefinedShape[6] = FVector(-this->VoxelRadius.X, this->VoxelRadius.Y, this->VoxelRadius.Z); /* Backward Bottom Right */
    DefinedShape[7] = FVector(-this->VoxelRadius.X, this->VoxelRadius.Y, -this->VoxelRadius.Z); /* Backward Top Right */
    
    TangentSetup = FProcMeshTangent(0.f, 1.f, 0.f); /* Front */
    this->AddQuad(DefinedShape[0], DefinedShape[1], DefinedShape[2], DefinedShape[3], TriangleIndexCount, TangentSetup);
    TangentSetup = FProcMeshTangent(1.f, 0.f, 0.f); /* Left */
    this->AddQuad(DefinedShape[2], DefinedShape[3], DefinedShape[4], DefinedShape[5], TriangleIndexCount, TangentSetup);
    TangentSetup = FProcMeshTangent(0.f, -1.f, 0.f); /* Back */
    this->AddQuad(DefinedShape[4], DefinedShape[5], DefinedShape[6], DefinedShape[7], TriangleIndexCount, TangentSetup);
    TangentSetup = FProcMeshTangent(-1.f, 0.f, 0.f); /* Right */
    this->AddQuad(DefinedShape[6], DefinedShape[7], DefinedShape[0], DefinedShape[1], TriangleIndexCount, TangentSetup);
    TangentSetup = FProcMeshTangent(0.f, 1.f, 0.f); /* Top */
    this->AddQuad(DefinedShape[6], DefinedShape[0], DefinedShape[4], DefinedShape[2], TriangleIndexCount, TangentSetup);
    TangentSetup = FProcMeshTangent(0.f, -1.f, 0.f); /* Bottom */
    this->AddQuad(DefinedShape[1], DefinedShape[7], DefinedShape[3], DefinedShape[5], TriangleIndexCount, TangentSetup);

    check(this->DevMaterial)
    this->ProceduralMeshComponent->SetMaterial(0, this->DevMaterial);
    this->ProceduralMeshComponent->CreateMeshSection(
        0,
        this->Vertices,
        this->Triangles,
        this->Normals,
        this->UVs,
        this->VertexColors,
        this->Tangents,
        false
    );
    
    return;
}

void APMCube::AddTriangle(const FVector& TopRight, const FVector& BottomRight, const FVector& BottomLeft, int32& TriIndex, const FProcMeshTangent& Tangent)
{
    const int32 Point1 = TriIndex++;
    const int32 Point2 = TriIndex++;
    const int32 Point3 = TriIndex++;

    this->Vertices.Add(TopRight);
    this->Vertices.Add(BottomRight);
    this->Vertices.Add(BottomLeft);

    this->Triangles.Add(Point1);
    this->Triangles.Add(Point2);
    this->Triangles.Add(Point3);

    const FVector Norm = FVector::CrossProduct(TopRight, BottomRight).GetSafeNormal();
    for (int i = 0; i < 3; i++)
    {
        this->Normals.Add(Norm);
        this->Tangents.Add(Tangent);
        this->VertexColors.Add(FColor(0, 0, 0, 0));

        continue;
    }
    
    this->UVs.Add(FVector2D(0.f, 1.f)); /* Top Right */
    this->UVs.Add(FVector2D(0.f, 0.f)); /* Bottom Right */
    this->UVs.Add(FVector2D(1.f, 0.f)); /* Bottom Left */
    
    return;
}

void APMCube::AddQuad(const FVector& TopRight, const FVector& BottomRight, const FVector& TopLeft, const FVector& BottomLeft, int32& TriIndex, const FProcMeshTangent& Tangent)
{
    const int32 Point1 = TriIndex++;
    const int32 Point2 = TriIndex++;
    const int32 Point3 = TriIndex++;
    const int32 Point4 = TriIndex++;

    this->Vertices.Add(TopRight);
    this->Vertices.Add(BottomRight);
    this->Vertices.Add(TopLeft);
    this->Vertices.Add(BottomLeft);

    this->Triangles.Add(Point1);
    this->Triangles.Add(Point2);
    this->Triangles.Add(Point3);

    this->Triangles.Add(Point4);
    this->Triangles.Add(Point3);
    this->Triangles.Add(Point2);

    const FVector Norm = FVector::CrossProduct(TopLeft - BottomRight, TopLeft - TopRight).GetSafeNormal();
    for (int i = 0; i < 4; i++)
    {
        this->Normals.Add(Norm);
        this->Tangents.Add(Tangent);
        this->VertexColors.Add(FColor(0, 0, 0, 3));

        continue;
    }
    
    this->UVs.Add(FVector2D(0.f, 1.f)); /* Top Left */
    this->UVs.Add(FVector2D(0.f, 0.f)); /* Bottom Left */
    this->UVs.Add(FVector2D(1.f, 1.f)); /* Top Right */
    this->UVs.Add(FVector2D(1.f, 0.f)); /* Bottom Right */

    return;
}
