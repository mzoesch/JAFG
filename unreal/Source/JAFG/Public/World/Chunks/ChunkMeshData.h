// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"

#include "ChunkMeshData.generated.h"

USTRUCT()
struct JAFG_API FChunkMeshData
{
	GENERATED_BODY()

public:

	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Normals;
	TArray<FColor> Colors;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;
	
public:

	void Clear();
	
};

inline void FChunkMeshData::Clear()
{
	this->Vertices.Empty();
	this->Triangles.Empty();
	this->Normals.Empty();
	this->Colors.Empty();
	this->UV0.Empty();
	this->Tangents.Empty(); /* TODO ?? Remove? */

	return;
}
