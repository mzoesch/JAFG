// © 2023 mzoesch. All rights reserved.

#include "Lib/Cuboid.h"

#include "ProceduralMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Core/CH_Master.h"
#include "Core/GI_Master.h"

#define GI CastChecked<UGI_Master>(this->GetGameInstance())

ACuboid::ACuboid()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->Mesh = this->CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->Mesh->SetCastShadow(false);
    this->SetRootComponent(this->Mesh);

    this->bHasCollisionConvexMesh = false;
    this->bHasPawnCollision = false;
    
    this->Voxel = EWorldVoxel::WV_Null;

    this->TriangleIndexCounter = 0;

    this->SphereComponent = this->CreateDefaultSubobject<USphereComponent>(TEXT("SpehereComponent"));
    this->SphereComponent->InitSphereRadius(this->CollisionSphereRadius);
    this->SphereComponent->SetupAttachment(this->Mesh);
    this->SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    this->SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACuboid::OnSphereComponentOverlapBegin);
    
    return;
}

void ACuboid::BeginPlay()
{
    Super::BeginPlay();

    if (this->bHasCollisionConvexMesh == true)
    {
        this->Mesh->bUseComplexAsSimpleCollision = false;

        TArray<FVector> CollisionConvexMesh;
        CollisionConvexMesh.Add(FVector( this->ConvexX,  this->ConvexY,  this->ConvexZ)); /* Forward  Top    Right */
        CollisionConvexMesh.Add(FVector( this->ConvexX,  this->ConvexY, -this->ConvexZ)); /* Forward  Bottom Right */
        CollisionConvexMesh.Add(FVector( this->ConvexX, -this->ConvexY,  this->ConvexZ)); /* Forward  Top    Left  */
        CollisionConvexMesh.Add(FVector( this->ConvexX, -this->ConvexY, -this->ConvexZ)); /* Forward  Bottom Left  */
        CollisionConvexMesh.Add(FVector(-this->ConvexX, -this->ConvexY,  this->ConvexZ)); /* Backward Top    Left  */
        CollisionConvexMesh.Add(FVector(-this->ConvexX, -this->ConvexY, -this->ConvexZ)); /* Backward Bottom Left  */
        CollisionConvexMesh.Add(FVector(-this->ConvexX,  this->ConvexY,  this->ConvexZ)); /* Backward Top    Right */
        CollisionConvexMesh.Add(FVector(-this->ConvexX,  this->ConvexY, -this->ConvexZ)); /* Backward Bottom Right */
        this->Mesh->AddCollisionConvexMesh(CollisionConvexMesh);

        this->Mesh->SetSimulatePhysics(true);
        this->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    }

    if (this->bHasPawnCollision)
    {
        this->SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        this->SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        this->SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    }
    
    return;
}

void ACuboid::GenerateMesh(const int V)
{
    this->GenerateMesh(FAccumulated(V));
    return;
}

void ACuboid::GenerateMesh(const FAccumulated Accumulated)
{
    this->Voxel = Accumulated.Accumulated;
    
    this->GenerateMesh();

    return;
}

void ACuboid::OnSphereComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ACH_Master::StaticClass()) == false)
    {
        return;
    }

    // TODO
    //      This ofc has to move to the AActor class. And then we need to check if
    //      the AActor has a specific interface and calls the method from the interface.

    if (ACH_Master* Character = CastChecked<ACH_Master>(OtherActor); Character->AddToInventory(FAccumulated(this->Voxel, 1), true))
    {
        this->Destroy();
    }

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

    if (this->Voxel == EWorldVoxel::WV_Null)
    {
        this->ApplyMesh();
        return;
    }
    
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
        this->Colors.Add(FColor(0, 0, 0, GI->GetTextureIndex(this->Voxel, TextureNormal)));

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
    this->Mesh->SetMaterial(0, GI->MDynamicOpaque);
    this->Mesh->CreateMeshSection(0, this->Vertices, this->Triangles, this->Normals, this->UVs, this->Colors, this->Tangents, false);
    return;
}

void ACuboid::SetAccumulated(const FAccumulated Accumulated)
{
    this->Voxel = Accumulated.Accumulated;

    return;
}

#undef GI
