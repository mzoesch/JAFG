// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/OakLogVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UOakLogVoxel::Initialize(UGI_Master* GI)
{
    GI->AddVoxelMask(FVoxelMask("JAFG", "OakLogVoxel", false, 0, nullptr));
    return;
}
