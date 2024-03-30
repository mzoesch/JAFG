// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "VoxelMask.generated.h"

class IVoxel;

USTRUCT()
struct JAFG_API FVoxelMask
{
	GENERATED_BODY()

public:

	FVoxelMask() = default;
	explicit FVoxelMask(const FString& NameSpace, const FString& Name);

	static const FVoxelMask Null;
	static const FVoxelMask Air;

	FString NameSpace;
	FString Name;
};
