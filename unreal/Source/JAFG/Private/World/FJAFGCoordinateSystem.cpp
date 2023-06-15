
#include "World/FJAFGCoordinateSystem.h"

FJAFGCoordinateSystem::FJAFGCoordinateSystem() {
	this->X = "0";
	this->Y = "0";
	this->Z = "0";

	return;
}

FJAFGCoordinateSystem::FJAFGCoordinateSystem(FVector UnrealVector) {
	UnrealVector.X < 0 && UnrealVector.X > -100 ?
		this->X = "-0"
		:
		this->X = FString::FromInt(UnrealVector.X * FJAFGCoordinateSystem::UnrealToJAFGCoordinateSystemScale)
		;

	UnrealVector.Y < 0 && UnrealVector.Y > -100 ?
		this->Y = "-0"
		:
		this->Y = FString::FromInt(UnrealVector.Y * FJAFGCoordinateSystem::UnrealToJAFGCoordinateSystemScale)
		;

	UnrealVector.Z < 0 && UnrealVector.Z > -100 ?
		this->Z = "-0"
		:
		this->Z = FString::FromInt(UnrealVector.Z * FJAFGCoordinateSystem::UnrealToJAFGCoordinateSystemScale)
		;
	
	return;
}

FJAFGCoordinateSystem::~FJAFGCoordinateSystem()
{
}

FString FJAFGCoordinateSystem::ToString() {
	return FString::Printf(
		TEXT("X : %s, Y : %s, Z : %s"),
		*this->X,
		*this->Y,
		*this->Z
	);
}

#pragma region Manipulation

void FJAFGCoordinateSystem::IncreaseToNorth() {
	if (this->X.Equals("-1"))
		this->X = "-0";
	else if (this->X.Equals("-0"))
		this->X = "0";
	else
		this->X = FString::FromInt(
			FCString::Atoi(*this->X) + 1
		);

	return;
}

void FJAFGCoordinateSystem::IncreaseToEast() {
	if (this->Y.Equals("-1"))
		this->Y = "-0";
	else if (this->Y.Equals("-0"))
		this->Y = "0";
	else
		this->Y = FString::FromInt(
			FCString::Atoi(*this->Y) + 1
		);

	return;
}

void FJAFGCoordinateSystem::IncreaseUpwards() {
	if (this->Z.Equals("-1"))
		this->Z = "-0";
	else if (this->Z.Equals("-0"))
		this->Z = "0";
	else
		this->Z = FString::FromInt(
			FCString::Atoi(*this->Z) + 1
		);

	return;
}

void FJAFGCoordinateSystem::IncreaseToSouth() {
	if (this->X.Equals("0"))
		this->X = "-0";
	else
		this->X = FString::FromInt(
			FCString::Atoi(*this->X) - 1
		);

	return;
}

void FJAFGCoordinateSystem::IncreaseToWest() {
	if (this->Y.Equals("0"))
		this->Y = "-0";
	else
		this->Y = FString::FromInt(
			FCString::Atoi(*this->Y) - 1
		);

	return;
}

void FJAFGCoordinateSystem::IncreaseDownwards() {
	if (this->Z.Equals("0"))
		this->Z = "-0";
	else
		this->Z = FString::FromInt(
			FCString::Atoi(*this->Z) - 1
		);

	return;
}

#pragma endregion Manipulation
