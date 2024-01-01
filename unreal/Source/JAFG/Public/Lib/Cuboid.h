// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Lib/Item.h"
#include "World/Voxel.h"
#include "Lib/FAccumulated.h"

#include "Cuboid.generated.h"

class USphereComponent;
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
    class UProceduralMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* SphereComponent;

public:

    virtual void BeginPlay() override;
    
public:

    void GenerateMesh(const EItem I);
    UFUNCTION(BlueprintCallable, Category = "Mesh")
    void GenerateMesh(const EVoxel V);
    void GenerateMesh(const FAccumulated Accumulated);

    UFUNCTION()
    void OnSphereComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

    // TODO
    //      Do we have to calculate Tangents? See the Greedy Meshing Algorithm.
    //      Works without this? What are the drawbacks and benefits?

    bool bHasCollisionConvexMesh;
    bool bHasPawnCollision;
    
    EVoxel Voxel;
    EItem Item;

    int32 TriangleIndexCounter = 0;
    FVector PreDefinedShape[8];
    
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D> UVs;
    TArray<FColor> Colors;

    void GenerateMesh();
    void CreateQuadrilateral(const FVector& TopRight, const FVector& BottomRight, const FVector& TopLeft, const FVector& BottomLeft, const FProcMeshTangent& Tangent);
    void ApplyMesh() const;

    int GetTextureIndex(const EVoxel VToGet, const FVector& Normal) const;

public:

    inline bool GetHasCollisionConvexMesh() const { return this->bHasCollisionConvexMesh; }
    inline void SetHasCollisionConvexMesh(const bool B) { this->bHasCollisionConvexMesh = B; }
    inline bool GetHasPawnCollision() const { return this->bHasPawnCollision; }
    inline void SetHasPawnCollision(const bool B) { this->bHasPawnCollision = B; }

public:

    void SetAccumulated(const FAccumulated Accumulated);
    
};
