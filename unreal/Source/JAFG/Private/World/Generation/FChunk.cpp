// © 2023 mzoesch. All rights reserved.


#include "World/Generation/FChunk.h"

#include "World/Blocks.h"
#include "World/FJAFGCoordinateSystemPrecise.h"

FChunk::FChunk() {
	this->X = 0;
	this->Y = 0;
	this->Z = 0;

	this->bIsLoaded = false;

	return;
}

FChunk::FChunk(int16 X, int16 Y, int16 Z) {
	this->X = X;
	this->Y = Y;
	this->Z = Z;

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

	const int16 _Z = FMath::Floor(
		FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(WorldPosition.Z) /
		FChunk::CHUNK_SIZE
	);

	return FChunk::FChunk(_X, _Y, _Z);	
}

FString FChunk::GetQuadrantID() const {
	return FString::Printf(
		TEXT("%s%s%s"),
		this->X >= 0 ? TEXT("Xp") : TEXT("Xn"),
		this->Y >= 0 ? TEXT("Yp") : TEXT("Yn"),
		this->Z >= 0 ? TEXT("Zp") : TEXT("Zn")
	);
}

FString FChunk::GetBlockID(FJAFGCoordinateSystem BlockPosition) {
	FChunk Bucket = FChunk::FromWorldPosition(BlockPosition.GetAsUnrealTransform().GetLocation());

	

	return Blocks::Grass;
}

FTransform FChunk::GetMiddleAsTransform() const {
	return FTransform(
		FVector(
			FJAFGCoordinateSystemPrecise::JAFGToUnrealCoorinateSystemScale * (
				( X * FChunk::CHUNK_SIZE ) + FChunk::CHUNK_SIZE * .5f
				),
			FJAFGCoordinateSystemPrecise::JAFGToUnrealCoorinateSystemScale * (
				( Y * FChunk::CHUNK_SIZE ) + FChunk::CHUNK_SIZE * .5f
				),
			0
		)
	);
}

TArray<FJAFGCoordinateSystem> FChunk::AllBlockPositions() const {
	TArray<FJAFGCoordinateSystem> Positions;

	for (int16 x = 0; x < FChunk::CHUNK_SIZE; x++) {
		for (int16 y = 0; y < FChunk::CHUNK_SIZE; y++) {
			for (int16 z = 0; z < FChunk::CHUNK_SIZE; z++) {
				
				// TO REMOVE
				if (z > 0)
					continue;

				FJAFGCoordinateSystem BlockPosition =
					FJAFGCoordinateSystem::FromChunkCoordinate(
						this->X, this->Y, this->Z,
						x, y, z
					)
					;

				Positions.Add(BlockPosition);
				continue;
			}

			continue;
		}

		continue;
	}

	return Positions;
}

bool FChunk::operator==(const FChunk& Other) const {
	if (this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z)
		return true;
	return false;
}

FString FChunk::ToString() const {
	return FString::Printf(
		TEXT("%d %d %d"),
		this->X, this->Y, this->Z
	);
}

FString FChunk::ToUniqueID() const {
	
	int16 absX = this->X;
	if (absX < 0)
		absX *= -1;
	FString _X = FString::FromInt(absX);
	while (_X.Len() < 6)
		_X = "0" + _X;
	if (this->X < 0)
		_X = "-" + _X;
	else
		_X = "+" + _X;
	
	int16 absY = this->Y;
	if (absY < 0)
		absY *= -1;
	FString _Y = FString::FromInt(absY);
	while (_Y.Len() < 6)
		_Y = "0" + _Y;
	if (this->Y < 0)
		_Y = "-" + _Y;
	else
		_Y = "+" + _Y;

	int16 absZ = this->Z;
	if (absZ < 0)
		absZ *= -1;
	FString _Z = FString::FromInt(absZ);
	while (_Z.Len() < 6)
		_Z = "0" + _Z;
	if (this->Z < 0)
		_Z = "-" + _Z;
	else
		_Z = "+" + _Z;
	
	return FString::Printf(
		TEXT("%s%s%s"),
		*_X, *_Y, *_Z
	);
}
