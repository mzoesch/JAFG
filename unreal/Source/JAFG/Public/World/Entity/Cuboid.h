// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Cuboid.generated.h"

class USphereComponent;
class UVoxelSubsystem;
class UProceduralMeshComponent;

struct JAFG_API FCuboidProceduralMeshData
{
    TArray<FVector>          Vertices;
    TArray<int32>            Triangles;
    TArray<FVector>          Normals;
    TArray<FColor>           Colors;
    TArray<FVector2D>        UV0;
    /**
     * What the matter with those tangents? Do we even need them? This is to be investigated.
     */
    TArray<FProcMeshTangent> Tangents;
};

UCLASS(NotBlueprintable)
class JAFG_API ACuboid : public AActor
{
    GENERATED_BODY()

public:

    explicit ACuboid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    FORCEINLINE auto SetAccumulatedIndex(const int32 InAccumulatedIndex) -> void { this->AccumulatedIndex = InAccumulatedIndex; }
    FORCEINLINE auto RegenerateProceduralMesh(void) -> void
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh();
        this->ApplyCollisionConvexMesh();
        this->ApplyProceduralMesh();

        return;
    }

protected:

    int32 AccumulatedIndex = 0;

    bool bHasCollisionConvexMesh = false;
    bool bHasPawnCollision = false;

    int32 CuboidX = ACuboid::DefaultCuboidX;
    int32 CuboidY = ACuboid::DefaultCuboidY;
    int32 CuboidZ = ACuboid::DefaultCuboidZ;
    int32 ConvexX = ACuboid::DefaultConvexX;
    int32 ConvexY = ACuboid::DefaultConvexY;
    int32 ConvexZ = ACuboid::DefaultConvexZ;

    int32 CollisionSphereRadius = ACuboid::DefaultCollisionSphereRadius;

    void AddForceToProceduralMesh(const FVector& Force) const;

    UFUNCTION()
    virtual void OnSphereComponentOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    ) PURE_VIRTUAL(ACuboid::OnSphereComponentOverlapBegin)

    UFUNCTION()
    virtual void OnSphereComponentOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex
    ) PURE_VIRTUAL(ACuboid::OnSphereComponentOverlapEnd)

private:

    /* All measurements in Unreal Coordinate System. */

    inline static constexpr int32 DefaultCuboidX { 100 };
    inline static constexpr int32 DefaultCuboidY { 100 };
    inline static constexpr int32 DefaultCuboidZ { 100 };
    inline static constexpr int32 DefaultConvexX { 100 };
    inline static constexpr int32 DefaultConvexY { 100 };
    inline static constexpr int32 DefaultConvexZ { 100 };

    inline static constexpr int32 DefaultCollisionSphereRadius { 100 };

    UPROPERTY()
    UVoxelSubsystem* VoxelSubsystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UProceduralMeshComponent* ProceduralMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* SphereComponent;

    FVector                           PreDefinedShape      [ 8 ];
    TArray<FCuboidProceduralMeshData> ProceduralMeshData;
    TArray<int>                       VertexCounts;

            auto GenerateProceduralMesh(void) -> void;
    virtual auto GenerateCollisionConvexMesh(TArray<FVector>& OutCollisionConvexMesh) -> void;
            auto ApplyCollisionConvexMesh(void) -> void;
            auto ApplyProceduralMesh(void) const -> void;
            auto ClearProceduralMesh(void) -> void;

    /**
     * Adds a quadrilateral to the mesh for the next render sweep.
     *
     * @param V1      Top Right Vertex
     * @param V2      Bottom Right Vertex
     * @param V3      Top Left Vertex
     * @param V4      Bottom Left Vertex
     * @param Tangent Non Obligatory Face Tangent.
     * @param Normal  Non Obligatory Face Normal.
     * @param Pixel   Non Obligatory Face Color if Texture is used to generate the mesh.
     *                The mesh of this quadrilateral must not be a voxel if set.
     */
    auto CreateQuadrilateral(
        const FVector& V1,
        const FVector& V2,
        const FVector& V3,
        const FVector& V4,
        const FProcMeshTangent& Tangent,
        const FVector& Normal,
        const FColor& Pixel
    ) -> void;

    /**
     * Adds a quadrilateral to the mesh for the next render sweep.
     *
     * @param V1      Top Right Vertex
     * @param V2      Bottom Right Vertex
     * @param V3      Top Left Vertex
     * @param V4      Bottom Left Vertex
     * @param Tangent Non Obligatory Face Tangent.
     * @param Normal  Non Obligatory Face Normal.
     */
    FORCEINLINE auto CreateQuadrilateral(
        const FVector& V1,
        const FVector& V2,
        const FVector& V3,
        const FVector& V4,
        const FProcMeshTangent& Tangent,
        const FVector& Normal
    ) -> void
    {
        this->CreateQuadrilateral(V1, V2, V3, V4, Tangent, Normal, FColor::Transparent);
    }
};
