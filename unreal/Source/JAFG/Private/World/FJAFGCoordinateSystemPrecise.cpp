// © 2023 mzoesch. All rights reserved.


#include "World/FJAFGCoordinateSystemPrecise.h"

FJAFGCoordinateSystemPrecise::FJAFGCoordinateSystemPrecise() {
	this->X = 0;
	this->Y = 0;
	this->Z = 0;

	return;
}

FJAFGCoordinateSystemPrecise::FJAFGCoordinateSystemPrecise(FVector UnrealVector) {
	this->X = UnrealVector.X;
	this->Y = UnrealVector.Y;
	this->Z = UnrealVector.Z;

	return;
}

FJAFGCoordinateSystemPrecise::~FJAFGCoordinateSystemPrecise()
{
}

FString FJAFGCoordinateSystemPrecise::ToString() {
	FString _X = FString::SanitizeFloat(
		this->X * FJAFGCoordinateSystemPrecise::UnrealToJAFGCoorinateSystemScale, 4);
	_X.RemoveAt(
		_X.Find(TEXT(".")) + 5, _X.Len() - _X.Find(TEXT(".")) + 5
	);

	FString _Y = FString::SanitizeFloat(
		this->Y * FJAFGCoordinateSystemPrecise::UnrealToJAFGCoorinateSystemScale, 4);
	_Y.RemoveAt(
		_Y.Find(TEXT(".")) + 5, _Y.Len() - _Y.Find(TEXT(".")) + 5
	);
	
	FString _Z = FString::SanitizeFloat(
		this->Z * FJAFGCoordinateSystemPrecise::UnrealToJAFGCoorinateSystemScale, 4);
	_Z.RemoveAt(
		_Z.Find(TEXT(".")) + 5, _Z.Len() - _Z.Find(TEXT(".")) + 5
	);

	return FString::Printf(
		TEXT("X : %s, Y : %s, Z : %s"),
		*_X, *_Y, *_Z
	);
}

double FJAFGCoordinateSystemPrecise::UnrealToJAFGCoordinateSystem(double UnrealCoordinate) {
	return UnrealCoordinate * FJAFGCoordinateSystemPrecise::UnrealToJAFGCoorinateSystemScale;
}
