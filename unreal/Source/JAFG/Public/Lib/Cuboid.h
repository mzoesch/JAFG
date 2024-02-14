// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Cuboid.generated.h"

class USphereComponent;
class UProceduralMeshComponent;

struct FTexture2DPixelMask
{
    FIntVector2     Pixel;
    FColor          Color;
};

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

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 TexX = 10;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 TexY = 10;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 TexZ = 10;
    
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 ConvexX = 100;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 ConvexY = 100;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    int32 ConvexZ = 100;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Mesh Params")
    float CollisionSphereRadius = 100.0f;
    
protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProceduralMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereComponent;

public:

    virtual void BeginPlay() override;
    
public:

    UFUNCTION(BlueprintCallable, Category = "Mesh")
    void GenerateMesh(const int InAccumulatedIndex);

    UFUNCTION()
    void OnSphereComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

    // TODO
    //      Do we have to calculate Tangents? See the Greedy Meshing Algorithm.
    //      Works without this? What are the drawbacks and benefits?

    bool bHasCollisionConvexMesh;
    bool bHasPawnCollision;

public:     int AccumulatedIndex;

private:    int32       TriangleIndexCounter = 0;
            FVector     PreDefinedShape[8];
    
    TArray<FVector>             Vertices;
    TArray<int32>               Triangles;
    TArray<FVector>             Normals;
    TArray<FProcMeshTangent>    Tangents;
    TArray<FVector2D>           UVs;
    TArray<FColor>              Colors;

    void GenerateMesh();
    void CreateVoxel();
    void CreateVoxelsBasedOnTexture();
    /**
     * Adds a quadrilateral to the mesh for the next render sweep.
     * 
     * @param V1        Top Right Vertex
     * @param V2        Bottom Right Vertex
     * @param V3        Top Left Vertex
     * @param V4        Bottom Left Vertex
     * @param Tangent   Non Obligatory Face Tangent,
     * @param Pixel     Non Obligatory Face Color if Texture is used to generate the mesh.
     *                  The mesh of this quadrilateral must not be a voxel if set.
     */
    void CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent, const FColor& Pixel);
    FORCEINLINE void CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent)
    { this->CreateQuadrilateral(V1, V2, V3, V4, Tangent, FColor::Transparent); }
    void ApplyMesh() const;

public:

    FORCEINLINE bool GetHasCollisionConvexMesh() const { return this->bHasCollisionConvexMesh; }
    FORCEINLINE void SetHasCollisionConvexMesh(const bool B) { this->bHasCollisionConvexMesh = B; }
    FORCEINLINE bool GetHasPawnCollision() const { return this->bHasPawnCollision; }
    FORCEINLINE void SetHasPawnCollision(const bool B) { this->bHasPawnCollision = B; }
};
