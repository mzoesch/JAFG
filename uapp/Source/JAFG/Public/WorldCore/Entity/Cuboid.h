// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "Cuboid.generated.h"

JAFG_VOID

class UTextureSubsystem;
class UVoxelSubsystem;
class UProceduralMeshComponent;

/**
 * Provides basic functionality for creating a single cuboid mesh.
 * Allows implementation on AActors or AActorComponents.
 */
UINTERFACE()
class JAFG_API UCuboidInterface : public UInterface
{
    GENERATED_BODY()
};

class JAFG_API ICuboidInterface
{
    GENERATED_BODY()

/** Has to be declared first, directly after unreal's GENERATED_BODY() macro. */
#define GENERATED_CUBOID_INTERFACE_BODY()                                               \
    public:                                                                             \
    FORCEINLINE virtual auto GenerateMesh(const voxel_t InAccumulated) -> void override \
    {                                                                                   \
        ICuboidInterface::GenerateMesh(InAccumulated, this->MeshComponent);             \
    }                                                                                   \
    private:
/** Has to be declared first, directly after unreal's GENERATED_BODY() macro. */
#define GENERATED_CUBOID_INTERFACE_BODY_WITH_MESH_COMP(MeshComp)                        \
    public:                                                                             \
    FORCEINLINE virtual auto GenerateMesh(const voxel_t InAccumulated) -> void override \
    {                                                                                   \
        ICuboidInterface::GenerateMesh(InAccumulated, MeshComp);                        \
    }                                                                                   \
    private:

public:

    int32 CuboidX = 8;
    int32 CuboidY = 8;
    int32 CuboidZ = 8;

    FORCEINLINE auto SetCuboidVoxelDimensions(const int32 InCuboidX, const int32 InCuboidY, const int32 InCuboidZ) -> void
    {
        this->CuboidX = InCuboidX;
        this->CuboidY = InCuboidY;
        this->CuboidZ = InCuboidZ;

        return;
    }

    FORCEINLINE auto SetCuboidVoxelDimensions(const int32 InCuboidXYZ) -> void
    {
        this->SetCuboidVoxelDimensions(InCuboidXYZ, InCuboidXYZ, InCuboidXYZ);
    }

    int32 TexX = 1;
    int32 TexY = 1;
    int32 TexZ = 1;

    FORCEINLINE auto SetCuboidTextureDimensions(const int32 InCuboidTextureX, const int32 InCuboidTextureY, const int32 InCuboidTextureZ) -> void
    {
        this->TexX = InCuboidTextureX;
        this->TexY = InCuboidTextureY;
        this->TexZ = InCuboidTextureZ;

        return;
    }

    FORCEINLINE auto SetCuboidTextureDimensions(const int32 InCuboidTextureXYZ) -> void
    {
        this->SetCuboidTextureDimensions(InCuboidTextureXYZ, InCuboidTextureXYZ, InCuboidTextureXYZ);
    }

    /* Do not override manually but with the generated body. */
    virtual void GenerateMesh(const voxel_t InAccumulated) PURE_VIRTUAL(ICuboidInterface::GenerateMesh);

protected:

    virtual void GenerateMesh(const voxel_t InAccumulated, UProceduralMeshComponent* InMeshComponent);

    TObjectPtr<UVoxelSubsystem>    VoxelSubsystem    = nullptr;
    TObjectPtr<UMaterialSubsystem> MaterialSubsystem = nullptr;
    TObjectPtr<UTextureSubsystem>  TextureSubsystem  = nullptr;

    /**
     * Variable is only used as a placeholder. The getter can be safely overriden in a derived class without
     * any struggle or side effects.
     */
    voxel_t CurrentAccumulated = Accumulated::Null.AccumulatedIndex;
    virtual auto GetCurrentAccumulatedIndex(void) const -> voxel_t { return this->CurrentAccumulated; }

    int32 TriangleIndexCounter = 0;

    TArray<FVector>          Vertices;
    TArray<int32>            Triangles;
    TArray<FVector>          Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D>        UVs;
    TArray<FColor>           Colors;

    void ApplyMesh(UProceduralMeshComponent* InMeshComponent) const;
    /**
     * Adds a quadrilateral to the mesh for the next render sweep.
     *
     * @param V1      Top    Right Vertex
     * @param V2      Bottom Right Vertex
     * @param V3      Top    Left  Vertex
     * @param V4      Bottom Left  Vertex
     * @param Tangent Face Tangent
     * @param Pixel   Non-obligatory face color.
     */
    void CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent, const TOptional<const FColor>& Pixel);
    FORCEINLINE
    void CreateQuadrilateral(const FVector& V1, const FVector& V2, const FVector& V3, const FVector& V4, const FProcMeshTangent& Tangent)
    {
        this->CreateQuadrilateral(V1, V2, V3, V4, Tangent, TOptional<const FColor>());
    }

private:

    void GenerateMeshForVoxels(void);
    void GenerateMeshForItems(void);
};

UCLASS(NotBlueprintable)
class JAFG_API ACuboid : public AActor, public ICuboidInterface
{
    GENERATED_BODY()
    GENERATED_CUBOID_INTERFACE_BODY()

public:

    explicit ACuboid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProceduralMeshComponent* MeshComponent = nullptr;
};

UCLASS(NotBlueprintable)
class JAFG_API UCuboidComponent : public UProceduralMeshComponent, public ICuboidInterface
{
    GENERATED_BODY()
    GENERATED_CUBOID_INTERFACE_BODY_WITH_MESH_COMP(this)

public:

    virtual void BeginPlay(void) override;
};
