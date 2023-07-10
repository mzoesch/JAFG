// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FChunk.generated.h"


USTRUCT()
struct JAFG_API FChunk {
public:
	GENERATED_BODY()

public:

	FChunk();
	FChunk(int16 X, int16 Y);
	~FChunk();

public:

	static const inline uint8 CHUNK_SIZE = 16;

public:

	int16 X;
	int16 Y;

	bool bIsLoaded;

public:

	static FChunk FromWorldPosition(const FVector& WorldPosition);

public:

	FTransform GetMiddleAsTransform() const;

public:

	bool operator==(const FChunk& Other) const;

public:

	FString ToString() const;

};
