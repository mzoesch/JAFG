// © 2023 mzoesch. All rights reserved.

#include "Lib/Cuboid.h"

#include "ProceduralMeshComponent.h"
#include "Core/GI_Master.h"

ACuboid::ACuboid()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->Mesh = this->CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->Mesh->SetCastShadow(false);
    this->SetRootComponent(this->Mesh);

    this->TriangleIndexCounter = 0;

    this->Voxel = EVoxel::Null;
    
    return;
}

void ACuboid::GenerateMesh(const EVoxel VToGenerate)
{
    this->Voxel = VToGenerate;
    this->GenerateMesh();
    return;
}

void ACuboid::GenerateMesh()
{
    this->Vertices.Reset();
    this->Triangles.Reset();
    this->Normals.Reset();
    this->Tangents.Reset();
    this->UVs.Reset();
    this->Colors.Reset();

    this->TriangleIndexCounter = 0;

    this->PreDefinedShape[0] = FVector( this->CuboidX,  this->CuboidY,  this->CuboidZ); /* Forward  Top    Right */
    this->PreDefinedShape[1] = FVector( this->CuboidX,  this->CuboidY, -this->CuboidZ); /* Forward  Bottom Right */
    this->PreDefinedShape[2] = FVector( this->CuboidX, -this->CuboidY,  this->CuboidZ); /* Forward  Top    Left  */
    this->PreDefinedShape[3] = FVector( this->CuboidX, -this->CuboidY, -this->CuboidZ); /* Forward  Bottom Left  */
    this->PreDefinedShape[4] = FVector(-this->CuboidX, -this->CuboidY,  this->CuboidZ); /* Backward Top    Left  */
    this->PreDefinedShape[5] = FVector(-this->CuboidX, -this->CuboidY, -this->CuboidZ); /* Backward Bottom Left  */
    this->PreDefinedShape[6] = FVector(-this->CuboidX,  this->CuboidY,  this->CuboidZ); /* Backward Top    Right */
    this->PreDefinedShape[7] = FVector(-this->CuboidX,  this->CuboidY, -this->CuboidZ); /* Backward Bottom Right */
    this->CreateQuadrilateral( this->PreDefinedShape[0], this->PreDefinedShape[1], this->PreDefinedShape[2], this->PreDefinedShape[3], FProcMeshTangent( 0.f,  1.f, 0.f) ); /* Front  */ 
    this->CreateQuadrilateral( this->PreDefinedShape[2], this->PreDefinedShape[3], this->PreDefinedShape[4], this->PreDefinedShape[5], FProcMeshTangent( 1.f,  0.f, 0.f) ); /* Left   */ 
    this->CreateQuadrilateral( this->PreDefinedShape[4], this->PreDefinedShape[5], this->PreDefinedShape[6], this->PreDefinedShape[7], FProcMeshTangent( 0.f, -1.f, 0.f) ); /* Back   */ 
    this->CreateQuadrilateral( this->PreDefinedShape[6], this->PreDefinedShape[7], this->PreDefinedShape[0], this->PreDefinedShape[1], FProcMeshTangent(-1.f,  0.f, 0.f) ); /* Right  */ 
    this->CreateQuadrilateral( this->PreDefinedShape[6], this->PreDefinedShape[0], this->PreDefinedShape[4], this->PreDefinedShape[2], FProcMeshTangent( 0.f,  1.f, 0.f) ); /* Top    */ 
    this->CreateQuadrilateral( this->PreDefinedShape[1], this->PreDefinedShape[7], this->PreDefinedShape[3], this->PreDefinedShape[5], FProcMeshTangent( 0.f, -1.f, 0.f) ); /* Bottom */ 

    this->ApplyMesh();
    
    return;
}

void ACuboid::CreateQuadrilateral(const FVector& TopRight, const FVector& BottomRight, const FVector& TopLeft, const FVector& BottomLeft, const FProcMeshTangent& Tangent)
{
    const int32 P1 = this->TriangleIndexCounter++;
    const int32 P2 = this->TriangleIndexCounter++;
    const int32 P3 = this->TriangleIndexCounter++;
    const int32 P4 = this->TriangleIndexCounter++;

    this->Vertices.Add(TopRight);
    this->Vertices.Add(BottomRight);
    this->Vertices.Add(TopLeft);
    this->Vertices.Add(BottomLeft);

    this->Triangles.Add(P1);
    this->Triangles.Add(P2);
    this->Triangles.Add(P3);

    this->Triangles.Add(P4);
    this->Triangles.Add(P3);
    this->Triangles.Add(P2);

    const FVector Normal = FVector::CrossProduct(TopLeft - BottomRight, TopLeft - TopRight).GetSafeNormal();
    for (int i = 0; i < 4; i++)
    {
        this->Normals.Add(Normal);
        this->Tangents.Add(Tangent);
        FVector TextureNormal = Normal;
        if (TextureNormal == FVector::UpVector)
        {
            TextureNormal = FVector::DownVector;
        }
        else if (TextureNormal == FVector::DownVector)
        {
            TextureNormal = FVector::UpVector;
        }
        this->Colors.Add(FColor(0, 0, 0, this->GetTextureIndex(this->Voxel, TextureNormal)));

        continue;
    }

    this->UVs.Append({
        FVector2D(1.0f, 1.0f), /* Top    Left  */
        FVector2D(1.0f, 0.0f), /* Bottom Left  */
        FVector2D(0.0f, 1.0f), /* Top    Right */
        FVector2D(0.0f, 0.0f), /* Bottom Right */
    });
    
    return;
}

void ACuboid::ApplyMesh() const
{
    if (this->Voxel == EVoxel::Null)
    {
        return;
    }

    this->Mesh->SetMaterial(0, this->Voxel == EVoxel::Glass || this->Voxel == EVoxel::Leaves ? CastChecked<UGI_Master>(this->GetGameInstance())->TranslucentMaterial : CastChecked<UGI_Master>(this->GetGameInstance())->DevMaterial);

    this->Mesh->CreateMeshSection(
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

int ACuboid::GetTextureIndex(const EVoxel VToGet, const FVector& Normal) const
{
    switch (VToGet)
    {
    case EVoxel::Stone: return 0;
    case EVoxel::Dirt: return 1;
    case EVoxel::Grass:
        {
            if (Normal == FVector::DownVector)
            {
                return 1;
            }
			
            if (Normal == FVector::UpVector)
            {
                return 2;
            }

            return 3;
        }
    case EVoxel::Glass: return 0;
    case EVoxel::Log:
        {
            if (Normal == FVector::UpVector || Normal == FVector::DownVector)
            {
                return 5;
            }

            return 4;
        }
    case EVoxel::Planks: return 6;
    case EVoxel::Leaves: return 1;
    default: return 255;
    }
}
