// © 2023 mzoesch. All rights reserved.
// This struct is for block coordinates only. Do not use this for precise calculations,
// such as entity positions. Use FJAFGCoordinateSystemPrecise instead.
// This struct has the ability to differentiate between 0 and -0.

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

	static FJAFGCoordinateSystem FromChunkCoordinate(
		const int16 CX, const int16 CY, const int16 CZ,
		const int16 _X, const int16 _Y, const int16 _Z
	);

public:

	FString ToString() const;
	FTransform GetAsUnrealTransform() const;

public:

	UPROPERTY()
	FString X;

	UPROPERTY()
	FString Y;
	
	UPROPERTY()
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
	static const inline float UnrealBlockOffset = 50.f;
	static const inline float UnrealBlockSize = 100.f;

};
