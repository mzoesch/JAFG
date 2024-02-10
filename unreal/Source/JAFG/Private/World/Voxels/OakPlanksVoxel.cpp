// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/OakPlanksVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UOakPlanksVoxel::Initialize(UGI_Master* GI)
{
    GI->AddVoxelMask(FVoxelMask("JAFG", "OakPlanksVoxel", false, 0, nullptr));
    return;
}
