// © 2023 mzoesch. All rights reserved.


#include "World/Generation/FChunk.h"

#include "World/FJAFGCoordinateSystemPrecise.h"

FChunk::FChunk() {
	this->X = 0;
	this->Y = 0;

	this->bIsLoaded = false;

	return;
}

FChunk::FChunk(int16 X, int16 Y) {
	this->X = X;
	this->Y = Y;

	this->bIsLoaded = false;

	return;
}

FChunk::~FChunk()
{
}

FChunk FChunk::FromWorldPosition(const FVector& WorldPosition) { 
	const int16 _X = FMath::Floor(
		FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(WorldPosition.X) /
		FChunk::CHUNK_SIZE
	);

	const int16 _Y = FMath::Floor(
	FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(WorldPosition.Y) /
		FChunk::CHUNK_SIZE
	);

	return FChunk::FChunk(_X, _Y);	
}

FTransform FChunk::GetMiddleAsTransform() const {
	return FTransform(
		FVector(
			FJAFGCoordinateSystemPrecise::JAFGToUnrealCoorinateSystemScale * ( ( X * CHUNK_SIZE ) + CHUNK_SIZE * .5f),
			FJAFGCoordinateSystemPrecise::JAFGToUnrealCoorinateSystemScale * ( ( Y * CHUNK_SIZE ) + CHUNK_SIZE * .5f),
			0
		)
	);
}

bool FChunk::operator==(const FChunk& Other) const {
	if (X == Other.X && Y == Other.Y)
		return true;
	return false;
}

FString FChunk::ToString() const {
	return FString::Printf(TEXT("%d %d"), X, Y);
}
