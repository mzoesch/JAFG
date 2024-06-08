// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Entity/Cuboid.h"

#include "ProceduralMeshComponent.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"

ACuboid::ACuboid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->MeshComponent = ObjectInitializer.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->MeshComponent);
    this->MeshComponent->SetCastShadow(false);

    return;
}

void ACuboid::BeginPlay(void)
{
    Super::BeginPlay();

    this->VoxelSubsystem    = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();    check( this->VoxelSubsystem )
    this->MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>(); check( this->MaterialSubsystem )

    return;
}

void ACuboid::GenerateMesh(const voxel_t InAccumulated)
{
    this->CurrentAccumulated = InAccumulated;

    this->TriangleIndexCounter = 0;

    this->Vertices.Reset();
    this->Triangles.Reset();
    this->Normals.Reset();
    this->Tangents.Reset();
    this->UVs.Reset();
    this->Colors.Reset();

    if (this->GetCurrentAccumulatedIndex() == ECommonVoxels::Null)
    {
        this->ApplyMesh();
        return;
    }

    FVector PreDefinedShape[8];

    PreDefinedShape[0] = FVector( this->CuboidX,  this->CuboidY,  this->CuboidZ ); /* Forward  Top    Right */
    PreDefinedShape[1] = FVector( this->CuboidX,  this->CuboidY, -this->CuboidZ ); /* Forward  Bottom Right */
    PreDefinedShape[2] = FVector( this->CuboidX, -this->CuboidY,  this->CuboidZ ); /* Forward  Top    Left  */
    PreDefinedShape[3] = FVector( this->CuboidX, -this->CuboidY, -this->CuboidZ ); /* Forward  Bottom Left  */
    PreDefinedShape[4] = FVector(-this->CuboidX, -this->CuboidY,  this->CuboidZ ); /* Backward Top    Left  */
    PreDefinedShape[5] = FVector(-this->CuboidX, -this->CuboidY, -this->CuboidZ ); /* Backward Bottom Left  */
    PreDefinedShape[6] = FVector(-this->CuboidX,  this->CuboidY,  this->CuboidZ ); /* Backward Top    Right */
    PreDefinedShape[7] = FVector(-this->CuboidX,  this->CuboidY, -this->CuboidZ ); /* Backward Bottom Right */

    this->CreateQuadrilateral( PreDefinedShape[0], PreDefinedShape[1], PreDefinedShape[2], PreDefinedShape[3], FProcMeshTangent( 0.0f,  1.0f, 0.0f ) ); /* Front  */
    this->CreateQuadrilateral( PreDefinedShape[2], PreDefinedShape[3], PreDefinedShape[4], PreDefinedShape[5], FProcMeshTangent( 1.0f,  0.0f, 0.0f ) ); /* Left   */
    this->CreateQuadrilateral( PreDefinedShape[4], PreDefinedShape[5], PreDefinedShape[6], PreDefinedShape[7], FProcMeshTangent( 0.0f, -1.0f, 0.0f ) ); /* Back   */
    this->CreateQuadrilateral( PreDefinedShape[6], PreDefinedShape[7], PreDefinedShape[0], PreDefinedShape[1], FProcMeshTangent(-1.0f,  0.0f, 0.0f ) ); /* Right  */
    this->CreateQuadrilateral( PreDefinedShape[6], PreDefinedShape[0], PreDefinedShape[4], PreDefinedShape[2], FProcMeshTangent( 0.0f,  1.0f, 0.0f ) ); /* Top    */
    this->CreateQuadrilateral( PreDefinedShape[1], PreDefinedShape[7], PreDefinedShape[3], PreDefinedShape[5], FProcMeshTangent( 0.0f, -1.0f, 0.0f ) ); /* Bottom */

    this->ApplyMesh();

    return;
}

void ACuboid::ApplyMesh(void) const
{
    this->MeshComponent->SetMaterial(0, this->MaterialSubsystem->MDynamicGroups[0]);
    this->MeshComponent->CreateMeshSection(
        0,
        this->Vertices,
        this->Triangles,
        this->Normals,
        this->UVs,
        this->Colors,
        this->Tangents,
        false
    );

    return;
}

void ACuboid::CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent)
{
    const int32 P1 = this->TriangleIndexCounter++;
    const int32 P2 = this->TriangleIndexCounter++;
    const int32 P3 = this->TriangleIndexCounter++;
    const int32 P4 = this->TriangleIndexCounter++;

    this->Vertices.Add(V1);
    this->Vertices.Add(V2);
    this->Vertices.Add(V3);
    this->Vertices.Add(V4);

    this->Triangles.Add(P1);
    this->Triangles.Add(P2);
    this->Triangles.Add(P3);

    this->Triangles.Add(P4);
    this->Triangles.Add(P3);
    this->Triangles.Add(P2);

    FVector Normal = FVector::CrossProduct(V3 - V2, V3 - V1).GetSafeNormal();
    Normal = FVector( FMath::RoundToInt(Normal.X), FMath::RoundToInt(Normal.Y), FMath::RoundToInt(Normal.Z) );
    for (int i = 0; i < 4; i++)
    {
        this->Normals.Add(Normal);
        this->Tangents.Add(Tangent);
        this->Colors.Add(FColor(0, 0, 0, this->VoxelSubsystem->GetTextureIndex(this->GetCurrentAccumulatedIndex(), Normal)));

        continue;
    }

    this->UVs.Append({
        FVector2D(1.0f, 0.0f), /* Bottom Left  */
        FVector2D(1.0f, 1.0f), /* Top    Left  */
        FVector2D(0.0f, 0.0f), /* Bottom Right */
        FVector2D(0.0f, 1.0f), /* Top    Right */
    });

    return;
}
