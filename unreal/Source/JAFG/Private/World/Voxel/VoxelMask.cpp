// Copyright 2024 mzoesch. All rights reserved.

#include "World/Voxel/VoxelMask.h"

const FVoxelMask FVoxelMask::Null = FVoxelMask(TEXT("COMMON"), TEXT("NullVoxel"));
const FVoxelMask FVoxelMask::Air  = FVoxelMask(TEXT("COMMON"), TEXT("AirVoxel"));

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name)
{
	this->NameSpace = NameSpace;
	this->Name = Name;
}
