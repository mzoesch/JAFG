// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "ChunkMeshData.generated.h"

/*
 * Does this has to be a USTRUCT? Can we make it a struct?
 */
USTRUCT()
struct JAFG_API FChunkMeshData
{
	GENERATED_BODY()

	TArray<FVector> Vertices;
	TArray<int> Triangles; /* Can we change the int type? */
	TArray<FVector> Normals;
	TArray<FColor> Colors;
	TArray<FVector2D> UV0;
	/**
	* Do we need tangent calculations? They are very expensive to calculate but are not required currently.
	* We may need this later for lighting calculations? To be determined. 
	*/
	TArray<FProcMeshTangent> Tangents;

	FORCEINLINE void Clear(void)
	{
		this->Vertices.Empty();
		this->Triangles.Empty();
		this->Normals.Empty();
		this->Colors.Empty();
		this->UV0.Empty();
		this->Tangents.Empty();
	}
};
