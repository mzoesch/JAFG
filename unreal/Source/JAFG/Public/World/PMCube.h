// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"

#include "PMCube.generated.h"

class UProceduralMeshComponent;

UCLASS()
class JAFG_API APMCube : public AActor
{
    GENERATED_BODY()
    
public:	

    APMCube();

protected:

    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh params")
    FVector VoxelRadius = FVector(100.f, 100.f, 100.f);

protected:

    UPROPERTY(VisibleAnywhere)
    USceneComponent* SceneComponent;

    UPROPERTY(VisibleAnywhere)
    UProceduralMeshComponent* ProceduralMeshComponent;

    UPROPERTY(EditInstanceOnly, Category="Material")
    const TObjectPtr<UMaterialInterface> DevMaterial;
    
private:
    
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D> UVs;
    TArray<FColor> VertexColors;
    
    void GenerateMesh();
    void AddTriangle(const FVector& TopRight, const FVector& BottomRight, const FVector& BottomLeft, int32& TriIndex, const FProcMeshTangent& Tangent);
    void AddQuad(const FVector& TopRight, const FVector& BottomRight, const FVector& TopLeft, const FVector& BottomLeft, int32& TriIndex, const FProcMeshTangent& Tangent);
    
};
