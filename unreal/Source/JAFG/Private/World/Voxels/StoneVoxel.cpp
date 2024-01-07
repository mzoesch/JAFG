// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/StoneVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UStoneVoxel::Initialize(class UGI_Master* GI)
{
    GI->AddVoxelMask(FVoxelMask("JAFG", "StoneVoxel", false, 0, nullptr));
    return;
}
