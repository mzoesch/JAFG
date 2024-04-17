// Copyright 2024 mzoesch. All rights reserved.

#include "World/Entity/Cuboid.h"

#include "ProceduralMeshComponent.h"
#include "Jar/Accumulated.h"
#include "System/MaterialSubsystem.h"
#include "World/Voxel/VoxelSubsystem.h"

ACuboid::ACuboid(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->ProceduralMeshComponent = this->CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->SetRootComponent(this->ProceduralMeshComponent);
    this->ProceduralMeshComponent->SetCastShadow(false);

    this->AccumulatedIndex = Accumulated::Null.AccumulatedIndex;

    this->VertexCounts.Empty();
    this->ProceduralMeshData.Empty();

    return;
}

void ACuboid::BeginPlay(void)
{
    Super::BeginPlay();

    check( this->GetGameInstance() )
    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( this->VoxelSubsystem )

    this->VertexCounts.Empty();
    this->ProceduralMeshData.Empty();

    return;
}

void ACuboid::GenerateProceduralMesh(void)
{
    this->PreDefinedShape[0] = /* Forward  Top    Right */
        FVector( this->CuboidX,  this->CuboidY,  this->CuboidZ );
    this->PreDefinedShape[1] = /* Forward  Bottom Right */
        FVector( this->CuboidX,  this->CuboidY, -this->CuboidZ );
    this->PreDefinedShape[2] = /* Forward  Top    Left  */
        FVector( this->CuboidX, -this->CuboidY,  this->CuboidZ );
    this->PreDefinedShape[3] = /* Forward  Bottom Left  */
        FVector( this->CuboidX, -this->CuboidY, -this->CuboidZ );
    this->PreDefinedShape[4] = /* Backward Top    Left  */
        FVector(-this->CuboidX, -this->CuboidY,  this->CuboidZ );
    this->PreDefinedShape[5] = /* Backward Bottom Left  */
        FVector(-this->CuboidX, -this->CuboidY, -this->CuboidZ );
    this->PreDefinedShape[6] = /* Backward Top    Right */
        FVector(-this->CuboidX,  this->CuboidY,  this->CuboidZ );
    this->PreDefinedShape[7] = /* Backward Bottom Right */
        FVector(-this->CuboidX,  this->CuboidY, -this->CuboidZ );

    this->CreateQuadrilateral( /* Front  */
        this->PreDefinedShape[0], this->PreDefinedShape[1], this->PreDefinedShape[2], this->PreDefinedShape[3],
        FProcMeshTangent( 0.0f,  1.0f, 0.0f ),
        FVector( 1.0f,  0.0f,  0.0f )
    );
    this->CreateQuadrilateral( /* Left   */
        this->PreDefinedShape[2], this->PreDefinedShape[3], this->PreDefinedShape[4], this->PreDefinedShape[5],
        FProcMeshTangent( 1.0f,  0.0f, 0.0f ),
        FVector( 0.0f,  1.0f,  0.0f )
    );
    this->CreateQuadrilateral( /* Back   */
        this->PreDefinedShape[4], this->PreDefinedShape[5], this->PreDefinedShape[6], this->PreDefinedShape[7],
        FProcMeshTangent( 0.0f, -1.0f, 0.0f ),
        FVector(-1.0f,  0.0f,  0.0f )
    );
    this->CreateQuadrilateral( /* Right  */
        this->PreDefinedShape[6], this->PreDefinedShape[7], this->PreDefinedShape[0], this->PreDefinedShape[1],
        FProcMeshTangent(-1.0f,  0.0f, 0.0f ),
        FVector( 0.0f, -1.0f,  0.0f )
    );
    this->CreateQuadrilateral( /* Top    */
        this->PreDefinedShape[6], this->PreDefinedShape[0], this->PreDefinedShape[4], this->PreDefinedShape[2],
        FProcMeshTangent( 0.0f,  1.0f, 0.0f ),
        FVector( 0.0f,  0.0f,  1.0f )
    );
    this->CreateQuadrilateral( /* Bottom */
        this->PreDefinedShape[1], this->PreDefinedShape[7], this->PreDefinedShape[3], this->PreDefinedShape[5],
        FProcMeshTangent( 0.0f, -1.0f, 0.0f ),
        FVector( 0.0f,  0.0f, -1.0f )
    );

    return;
}

void ACuboid::ApplyProceduralMesh(void) const
{
    check( this->GetGameInstance() )
    const UMaterialSubsystem* MaterialSubsystem = this->GetGameInstance()->GetSubsystem<UMaterialSubsystem>();

    if (MaterialSubsystem == nullptr)
    {
        LOG_FATAL(LogGenPrevAssets, "Could not get Material Subsystem.")
        return;
    }

    for (int i = 0; i < this->ProceduralMeshData.Num(); ++i)
    {
        if (i == ETextureGroup::Opaque)
        {
            check( MaterialSubsystem->MDynamicOpaque )
            this->ProceduralMeshComponent->SetMaterial(
                ETextureGroup::Opaque,
                MaterialSubsystem->MDynamicOpaque
            );
        }
        else if (i == ETextureGroup::FullBlendOpaque)
        {
            check( MaterialSubsystem->MDynamicFullBlendOpaque )
            this->ProceduralMeshComponent->SetMaterial(
                ETextureGroup::FullBlendOpaque,
                MaterialSubsystem->MDynamicFullBlendOpaque
            );
        }
        else if (i == ETextureGroup::FloraBlendOpaque)
        {
            check( MaterialSubsystem->MDynamicFloraBlendOpaque )
            this->ProceduralMeshComponent->SetMaterial(
                ETextureGroup::FloraBlendOpaque,
                MaterialSubsystem->MDynamicFloraBlendOpaque
            );
        }
        else
        {
            LOG_ERROR(LogGenPrevAssets, "Texture group %d not implemented.", i)
            this->ProceduralMeshComponent->SetMaterial(
                i,
                MaterialSubsystem->MDynamicOpaque
            );
        }

        this->ProceduralMeshComponent->CreateMeshSection(
            i,
            this->ProceduralMeshData[i].Vertices,
            this->ProceduralMeshData[i].Triangles,
            this->ProceduralMeshData[i].Normals,
            this->ProceduralMeshData[i].UV0,
            this->ProceduralMeshData[i].Colors,
            this->ProceduralMeshData[i].Tangents,
            false
        );

        continue;
    }

    return;
}

void ACuboid::ClearProceduralMesh(void)
{
    for (int i = 0; i < this->ProceduralMeshData.Num(); ++i)
    {
        this->ProceduralMeshData[i].Vertices.Reset();
        this->ProceduralMeshData[i].Triangles.Reset();
        this->ProceduralMeshData[i].Normals.Reset();
        this->ProceduralMeshData[i].Colors.Reset();
        this->ProceduralMeshData[i].UV0.Reset();
        this->ProceduralMeshData[i].Tangents.Reset();

        continue;
    }

    for (int i = 0; i < this->VertexCounts.Num(); ++i)
    {
        this->VertexCounts[i] = 0;
        continue;
    }

    return;
}

void ACuboid::CreateQuadrilateral(
    const FVector& V1,
    const FVector& V2,
    const FVector& V3,
    const FVector& V4,
    const FProcMeshTangent& Tangent,
    const FVector& Normal,
    const FColor& Pixel
)
{
    /*
     * We should fix this some time in the future. Why do we have to flip everything???
     */
    FVector FlippedNormal = Normal;
    if (FlippedNormal == FVector::UpVector)
    {
        FlippedNormal = FVector::DownVector;
    }
    else if (FlippedNormal == FVector::DownVector)
    {
        FlippedNormal = FVector::UpVector;
    }

    const int TextureGroup = this->VoxelSubsystem->GetTextureGroup(this->AccumulatedIndex, FlippedNormal);

    if (TextureGroup > this->VertexCounts.Num() - 1)
    {
        const int Delta = TextureGroup - this->VertexCounts.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->VertexCounts.Add(0);
            continue;
        }
    }

    if (TextureGroup > this->ProceduralMeshData.Num() - 1)
    {
        const int Delta = TextureGroup - this->ProceduralMeshData.Num() + 1;
        for (int i = 0; i < Delta; ++i)
        {
            this->ProceduralMeshData.Add(FCuboidProceduralMeshData());
            continue;
        }
    }

    const int32 Point1 = this->VertexCounts[TextureGroup]++;
    const int32 Point2 = this->VertexCounts[TextureGroup]++;
    const int32 Point3 = this->VertexCounts[TextureGroup]++;
    const int32 Point4 = this->VertexCounts[TextureGroup]++;

    this->ProceduralMeshData[TextureGroup].Vertices.Append({V1, V2, V3, V4});

    this->ProceduralMeshData[TextureGroup].Triangles.Append({
        Point1, Point2, Point3, /* First  Triangle  */
        Point4, Point3, Point2  /* Second Triangle  */
    });

    const FVector CalculatedNormal = FVector::CrossProduct(V3 - V2, V3 - V1).GetSafeNormal();
    for (int i = 0; i < 4; i++)
    {
        this->ProceduralMeshData[TextureGroup].Normals.Add(CalculatedNormal);
        this->ProceduralMeshData[TextureGroup].Tangents.Add(Tangent);

        if (Pixel == FColor::Transparent)
        {
            this->ProceduralMeshData[TextureGroup].Colors.Add(FColor(0, 96, 0, this->VoxelSubsystem->GetTextureIndex(this->AccumulatedIndex, FlippedNormal)));
        }
        else
        {
            this->ProceduralMeshData[TextureGroup].Colors.Add(Pixel);
        }

        continue;
    }

    this->ProceduralMeshData[TextureGroup].UV0.Append({
        FVector2D(1.0f, 1.0f), /* Top    Left  */
        FVector2D(1.0f, 0.0f), /* Bottom Left  */
        FVector2D(0.0f, 1.0f), /* Top    Right */
        FVector2D(0.0f, 0.0f), /* Bottom Right */
    });
}
