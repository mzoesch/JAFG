// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "World/Generation/FChunk.h"

#include "FChunk2D.generated.h"


USTRUCT()
struct JAFG_API FChunk2D {
public:
	GENERATED_BODY()

public:

	FChunk2D();
	FChunk2D(int16 X, int16 Y);
	~FChunk2D();

public:

	static constexpr int CHUNK_SIZE{16};
	static constexpr int VERTICAL_SUB_CHUNKS{2};
	
public:

	UPROPERTY()
	int16 X;
	UPROPERTY()
	int16 Y;

public:

	static FChunk2D FromWorldPosition(const FVector& WorldPosition);

public:

	TArray<FChunk> AllSubChunks() const;

public:

	bool operator==(const FChunk2D& Other) const;
	
public:

	FString ToString() const;
	FString ToDBReadyString() const;
	
};
