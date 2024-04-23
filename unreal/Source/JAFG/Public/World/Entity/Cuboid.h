// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Cuboid.generated.h"

DECLARE_DELEGATE_SixParams(
    FOnCuboidBeginOverlapEventSignature,
    UPrimitiveComponent* /* OverlappedComponent */ ,
    AActor* /* OtherActor */ ,
    UPrimitiveComponent* /* OtherComp */,
    const int32 /* OtherBodyIndex */,
    const bool /* bFromSweep */,
    const FHitResult& /* SweepResult */
)

DECLARE_DELEGATE_FourParams(
    FOnCuboidEndOverlapEventSignature,
    UPrimitiveComponent* /* OverlappedComponent */ ,
    AActor* /* OtherActor */ ,
    UPrimitiveComponent* /* OtherComp */,
    const int32 /* OtherBodyIndex */
)

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

/**
 * A cuboid based on any accumulated item index than can be spawned in the UWorld
 * without the need of an owning chunk.
 * An AActor completely detached from the chunk subsystem.
 */
UCLASS(NotBlueprintable)
class JAFG_API ACuboid : public AActor
{
    GENERATED_BODY()

public:

    explicit ACuboid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    FORCEINLINE auto SetAccumulatedIndex(const int32 InAccumulatedIndex) -> void
    {
        this->AccumulatedIndex = InAccumulatedIndex;
    }

    FORCEINLINE auto RegenerateProceduralMesh(void) -> void
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh();
        this->ApplyCollisionConvexMesh();
        this->ApplyProceduralMesh();

        return;
    }

    FORCEINLINE auto SetHasCollisionConvexMesh(const bool bInHasCollisionConvexMesh) -> void
    {
        this->bHasCollisionConvexMesh = bInHasCollisionConvexMesh;
    }

    FORCEINLINE auto SetHasPawnCollision(const bool bInHasPawnCollision) -> void
    {
        this->bHasPawnCollision = bInHasPawnCollision;
    }

    FORCEINLINE auto SetCuboidDimensions(const int32 InCuboidX, const int32 InCuboidY, const int32 InCuboidZ) -> void
    {
        this->CuboidX = InCuboidX;
        this->CuboidY = InCuboidY;
        this->CuboidZ = InCuboidZ;

        return;
    }

    FORCEINLINE auto SetConvexDimensions(const int32 InConvexX, const int32 InConvexY, const int32 InConvexZ) -> void
    {
        this->ConvexX = InConvexX;
        this->ConvexY = InConvexY;
        this->ConvexZ = InConvexZ;

        return;
    }

                auto AddForceToProceduralMesh(const FVector& Force) const -> void;

    FOnCuboidBeginOverlapEventSignature OnCuboidBeginOverlapEvent;
    FOnCuboidEndOverlapEventSignature   OnCuboidEndOverlapEvent;

protected:

    int32 AccumulatedIndex       = 0;

    bool bHasCollisionConvexMesh = false;
    bool bHasPawnCollision       = false;

    int32 CuboidX = ACuboid::DefaultCuboidX;
    int32 CuboidY = ACuboid::DefaultCuboidY;
    int32 CuboidZ = ACuboid::DefaultCuboidZ;
    int32 ConvexX = ACuboid::DefaultConvexX;
    int32 ConvexY = ACuboid::DefaultConvexY;
    int32 ConvexZ = ACuboid::DefaultConvexZ;

    int32 CollisionSphereRadius = ACuboid::DefaultCollisionSphereRadius;

    UFUNCTION()
    void OnCuboidBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        const int32 OtherBodyIndex,
        const bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnCuboidEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        const int32 OtherBodyIndex
    );

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
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> SphereComponent;

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
