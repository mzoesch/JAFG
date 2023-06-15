// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FJAFGCoordinateSystem.generated.h"


USTRUCT()
struct JAFG_API FJAFGCoordinateSystem {
public:
	GENERATED_BODY()

public:

	FJAFGCoordinateSystem();
	FJAFGCoordinateSystem(FVector UnrealVector);
	~FJAFGCoordinateSystem();

public:

	FString ToString();

public:

	FString X;
	FString Y;
	FString Z;

public:

	void IncreaseToNorth();
	void IncreaseToEast();
	void IncreaseUpwards();
	void IncreaseToSouth();
	void IncreaseToWest();
	void IncreaseDownwards();

public:

	static const inline float UnrealToJAFGCoordinateSystemScale = 0.01f;
	static const inline float JAFGToUnrealCoordinateSystemScale = 100.f;

};
