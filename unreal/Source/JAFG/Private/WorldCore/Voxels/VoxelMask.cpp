// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Voxels/VoxelMask.h"

const FVoxelMask FVoxelMask::Null = FVoxelMask( CommonNamespace, TEXT("NullVoxel"), ETextureGroup::Core );
const FVoxelMask FVoxelMask::Air  = FVoxelMask( CommonNamespace, TEXT("AirVoxel"),  ETextureGroup::Core );
