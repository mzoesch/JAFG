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
	return FString::Printf(
		TEXT("%d%d"),
		this->X,
		this->Y
	);
}
