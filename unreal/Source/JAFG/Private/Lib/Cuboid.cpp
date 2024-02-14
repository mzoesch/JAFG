// © 2023 mzoesch. All rights reserved.

#include "Lib/Cuboid.h"

#include "ProceduralMeshComponent.h"
#include "Assets/General.h"
#include "Components/SphereComponent.h"

#include "Core/CH_Master.h"
#include "Core/GI_Master.h"
#include "Lib/FAccumulated.h"

#define GI CastChecked<UGI_Master>(this->GetGameInstance())
#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("%s: %s"), *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))

ACuboid::ACuboid()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->Mesh = this->CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
    this->Mesh->SetCastShadow(false);
    this->SetRootComponent(this->Mesh);

    this->bHasCollisionConvexMesh   = false;
    this->bHasPawnCollision         = false;
    this->AccumulatedIndex          = FAccumulated::NullAccumulated.Accumulated;
    this->TriangleIndexCounter      = 0;

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

void ACuboid::GenerateMesh(const int InAccumulatedIndex)
{
    this->AccumulatedIndex = InAccumulatedIndex;
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

    if (ACH_Master* Character = CastChecked<ACH_Master>(OtherActor); Character->AddToInventory(FAccumulated(this->AccumulatedIndex, 1), true))
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

    if (this->AccumulatedIndex == FAccumulated::NullAccumulated.Accumulated)
    {
        this->ApplyMesh();
        return;
    }

    if (FAccumulated(this->AccumulatedIndex).IsVoxel())
    {
        this->CreateVoxel();
    }
    else
    {
        this->CreateVoxelsBasedOnTexture();
    }
    
    this->ApplyMesh();
    
    return;
}

void ACuboid::CreateVoxel()
{
    this->PreDefinedShape[0] = FVector( this->CuboidX,  this->CuboidY,  this->CuboidZ ); /* Forward  Top    Right */
    this->PreDefinedShape[1] = FVector( this->CuboidX,  this->CuboidY, -this->CuboidZ ); /* Forward  Bottom Right */
    this->PreDefinedShape[2] = FVector( this->CuboidX, -this->CuboidY,  this->CuboidZ ); /* Forward  Top    Left  */
    this->PreDefinedShape[3] = FVector( this->CuboidX, -this->CuboidY, -this->CuboidZ ); /* Forward  Bottom Left  */
    this->PreDefinedShape[4] = FVector(-this->CuboidX, -this->CuboidY,  this->CuboidZ ); /* Backward Top    Left  */
    this->PreDefinedShape[5] = FVector(-this->CuboidX, -this->CuboidY, -this->CuboidZ ); /* Backward Bottom Left  */
    this->PreDefinedShape[6] = FVector(-this->CuboidX,  this->CuboidY,  this->CuboidZ ); /* Backward Top    Right */
    this->PreDefinedShape[7] = FVector(-this->CuboidX,  this->CuboidY, -this->CuboidZ ); /* Backward Bottom Right */
    
    this->CreateQuadrilateral( this->PreDefinedShape[0], this->PreDefinedShape[1], this->PreDefinedShape[2], this->PreDefinedShape[3], FProcMeshTangent( 0.0f,  1.0f, 0.0f )); /* Front  */ 
    this->CreateQuadrilateral( this->PreDefinedShape[2], this->PreDefinedShape[3], this->PreDefinedShape[4], this->PreDefinedShape[5], FProcMeshTangent( 1.0f,  0.0f, 0.0f )); /* Left   */ 
    this->CreateQuadrilateral( this->PreDefinedShape[4], this->PreDefinedShape[5], this->PreDefinedShape[6], this->PreDefinedShape[7], FProcMeshTangent( 0.0f, -1.0f, 0.0f )); /* Back   */ 
    this->CreateQuadrilateral( this->PreDefinedShape[6], this->PreDefinedShape[7], this->PreDefinedShape[0], this->PreDefinedShape[1], FProcMeshTangent(-1.0f,  0.0f, 0.0f )); /* Right  */ 
    this->CreateQuadrilateral( this->PreDefinedShape[6], this->PreDefinedShape[0], this->PreDefinedShape[4], this->PreDefinedShape[2], FProcMeshTangent( 0.0f,  1.0f, 0.0f )); /* Top    */ 
    this->CreateQuadrilateral( this->PreDefinedShape[1], this->PreDefinedShape[7], this->PreDefinedShape[3], this->PreDefinedShape[5], FProcMeshTangent( 0.0f, -1.0f, 0.0f )); /* Bottom */ 

    return;
}

void ACuboid::CreateVoxelsBasedOnTexture()
{
    UTexture2D* Tex = FGeneral::LoadTexture2D(FAccumulated(this->AccumulatedIndex));

    if (Tex == nullptr)
    {
        UIL_LOG(Fatal, TEXT("ACuboid::CreateVoxelsBasedOnTexture: Texture is null."));
        return;
    }

    TArray<FTexture2DPixelMask> Masks   = TArray<FTexture2DPixelMask>();
    FTexture2DMipMap*           Mip     = &Tex->GetPlatformData()->Mips[0];
    const uint32                Width   = Mip->SizeX;
    const uint32                Height  = Mip->SizeY;
    FByteBulkData*              Bulk    = &Mip->BulkData;
    const FColor*               Data    = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_ONLY));

    for (uint32 w = 0; w < Width; w++) { for (uint32 h = 0; h < Height; h++)
    {
        if (const FColor Pixel = Data[w + h * Width]; Pixel != FColor::Transparent && Pixel.A >= 255)
        {
            Masks.Add(FTexture2DPixelMask(FIntVector2(w, h), Pixel));
        }
    } }

    Bulk->Unlock();

    for (const auto [Pixel, Color] : Masks)
    {
        FVector P1 = FVector( this->TexX,  this->TexY,  this->TexZ ); /* Forward  Top    Right */
        FVector P2 = FVector( this->TexX,  this->TexY, -this->TexZ ); /* Forward  Bottom Right */
        FVector P3 = FVector( this->TexX, -this->TexY,  this->TexZ ); /* Forward  Top    Left  */
        FVector P4 = FVector( this->TexX, -this->TexY, -this->TexZ ); /* Forward  Bottom Left  */
        FVector P5 = FVector(-this->TexX, -this->TexY,  this->TexZ ); /* Backward Top    Left  */
        FVector P6 = FVector(-this->TexX, -this->TexY, -this->TexZ ); /* Backward Bottom Left  */
        FVector P7 = FVector(-this->TexX,  this->TexY,  this->TexZ ); /* Backward Top    Right */
        FVector P8 = FVector(-this->TexX,  this->TexY, -this->TexZ ); /* Backward Bottom Right */

        P1 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Forward  Top    Right - Space Shift */
        P2 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Forward  Bottom Right - Space Shift */
        P3 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Forward  Top    Left  - Space Shift */
        P4 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Forward  Bottom Left  - Space Shift */
        P5 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Backward Top    Left  - Space Shift */
        P6 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Backward Bottom Left  - Space Shift */
        P7 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Backward Top    Right - Space Shift */
        P8 += FVector( Pixel.X * this->TexX * 2,  Pixel.Y * this->TexY * 2, 0 ); /* Backward Bottom Right - Space Shift */

        /* Front */
        if (Masks.ContainsByPredicate( [&] (const FTexture2DPixelMask& Mask) { return Mask.Pixel == Pixel + FIntVector2( 1,  0 ); } ) == false)
        {
            this->CreateQuadrilateral( P1, P2, P3, P4, FProcMeshTangent( 0.f,  1.f, 0.f ), Color);
        }

        /* Left */
        if (Masks.ContainsByPredicate( [&] (const FTexture2DPixelMask& Mask) { return Mask.Pixel == Pixel + FIntVector2( 0, -1 ); } ) == false)
        {
            this->CreateQuadrilateral( P3, P4, P5, P6, FProcMeshTangent( 1.f,  0.f, 0.f ), Color);
        }

        /* Back */
        if (Masks.ContainsByPredicate( [&] (const FTexture2DPixelMask& Mask) { return Mask.Pixel == Pixel + FIntVector2(-1,  0 ); } ) == false)
        {
            this->CreateQuadrilateral( P5, P6, P7, P8, FProcMeshTangent( 0.f, -1.f, 0.f ), Color);
        }

        /* Right */
        if (Masks.ContainsByPredicate( [&] (const FTexture2DPixelMask& Mask) { return Mask.Pixel == Pixel + FIntVector2( 0,  1 ); } ) == false)
        {
            this->CreateQuadrilateral( P7, P8, P1, P2, FProcMeshTangent(-1.f,  0.f, 0.f ), Color);
        }
        
        this->CreateQuadrilateral( P7, P1, P5, P3, FProcMeshTangent( 0.f,  1.f, 0.f ), Color); /* Top    */
        this->CreateQuadrilateral( P2, P8, P4, P6, FProcMeshTangent( 0.f, -1.f, 0.f ), Color); /* Bottom */
        
        continue;
    }
    
    return;
}

void ACuboid::CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent, const FColor& Pixel)
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

    const FVector Normal = FVector::CrossProduct(V3 - V2, V3 - V1).GetSafeNormal();
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

        if (Pixel == FColor::Transparent)
        {
            this->Colors.Add(FColor(0, 0, 0, GI->GetTextureIndex(this->AccumulatedIndex, TextureNormal)));
        }
        else
        {
            this->Colors.Add(Pixel);
        }
        
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
    if (FAccumulated(this->AccumulatedIndex).IsVoxel())
    {
        this->Mesh->SetMaterial(0, GI->MDynamicOpaque);
    }
    else
    {
        this->Mesh->SetMaterial(0, GI->MItem);
    }
    
    this->Mesh->CreateMeshSection(0, this->Vertices, this->Triangles, this->Normals, this->UVs, this->Colors, this->Tangents, false);

    return;
}

#undef GI
#undef UIL_LOG
