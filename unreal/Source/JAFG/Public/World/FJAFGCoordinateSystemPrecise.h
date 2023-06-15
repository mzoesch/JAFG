// © 2023 mzoesch. All rights reserved.
// This struct does not allow +- zeros. Only use this for precise calculations
// and not block positions. For block positions use FJAFGCoordinateSystem.

#pragma once

#include "CoreMinimal.h"

#include "FJAFGCoordinateSystemPrecise.generated.h"


USTRUCT()
struct JAFG_API FJAFGCoordinateSystemPrecise {
public:
	GENERATED_BODY()

public:

	FJAFGCoordinateSystemPrecise();
	FJAFGCoordinateSystemPrecise(FVector UnrealVector);
	~FJAFGCoordinateSystemPrecise();

public:

	FString ToString();

public:

	double X;
	double Y;
	double Z;

public:

	static const inline float UnrealToJAFGCoorinateSystemScale = 0.01f;
	static const inline float JAFGToUnrealCoorinateSystemScale = 100.f;

};
