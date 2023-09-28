// © 2023 mzoesch. All rights reserved.

#include "World/Generation/FChunk2D.h"

#include "World/FJAFGCoordinateSystemPrecise.h"

FChunk2D::FChunk2D() {
	this->X = 0;
	this->Y = 0;

	return;
}

FChunk2D::FChunk2D(int16 X, int16 Y) {
	this->X = X;
	this->Y = Y;

	return;
}

FChunk2D::~FChunk2D()
{
}

FChunk2D FChunk2D::FromWorldPosition(const FVector& WorldPosition) {
	const int16 _X = FMath::Floor(
		FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(WorldPosition.X) /
		FChunk2D::CHUNK_SIZE
	);

	const int16 _Y = FMath::Floor(
		FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(WorldPosition.Y) /
		FChunk2D::CHUNK_SIZE
	);
	
	return FChunk2D::FChunk2D(_X, _Y);
}

TArray<FChunk> FChunk2D::AllSubChunks() const {
	TArray<FChunk> SubChunks;
	for (uint8 y = 0; y < FChunk2D::VERTICAL_SUB_CHUNKS; y++) {
		SubChunks.Add(
			FChunk(
				this->X,
				this->Y,
				y
			)
		);
		
		continue;
	}

	return SubChunks;
}

bool FChunk2D::operator==(const FChunk2D& Other) const {
	return this->X == Other.X && this->Y == Other.Y;
}

FString FChunk2D::ToString() const {
	return  FString::Printf(
		TEXT("%d %d"),
		this->X,
		this->Y
	);
}

FString FChunk2D::ToDBReadyString() const {
	FString _X = FString::FromInt(abs(this->X));
	while (_X.Len() < 6)
		_X = "0" + _X;
	if (this->X < 0)
		_X = "-" + _X;
	else
		_X = "+" + _X;
	
	FString _Y = FString::FromInt(abs(this->Y));
	while (_Y.Len() < 6)
		_Y = "0" + _Y;
	if (this->Y < 0)
		_Y = "-" + _Y;
	else
		_Y = "+" + _Y;
	
	return FString::Printf(
		TEXT("%s%s"),
		*_X,
		*_Y
	);
}
