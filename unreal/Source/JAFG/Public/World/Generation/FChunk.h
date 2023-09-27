// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../FJAFGCoordinateSystem.h"

#include "FChunk.generated.h"


USTRUCT()
struct JAFG_API FChunk {
public:
	GENERATED_BODY()

public:

	FChunk();
	FChunk(int16 X, int16 Y, int16 Z);
	~FChunk();

public:

	static constexpr int CHUNK_SIZE{16};

public:

	int16 X;
	int16 Y;
	int16 Z;

	bool bIsLoaded;

public:

	static FChunk FromWorldPosition(const FVector& WorldPosition);
	// static FChunk FromFJAFGCoordinateSystem(const FJAFGCoordinateSystem& JAFGCoordinate);

public:

	FString GetQuadrantID() const;
	static FString GetBlockID(FJAFGCoordinateSystem BlockPosition);

public:

	FTransform GetMiddleAsTransform() const;
	TArray<FJAFGCoordinateSystem> AllBlockPositions() const;
	
public:

	bool operator==(const FChunk& Other) const;

public:

	FString ToString() const;
	FString ToUniqueID() const;

public:

	TArray<FChunk> GetNeighbours2D() const;
	
};
