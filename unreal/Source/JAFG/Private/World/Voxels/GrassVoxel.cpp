// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/GrassVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UGrassVoxel::Initialize(class UGI_Master* GI)
{
    GI->AddVoxelMask(FVoxelMask("JAFG", "GrassVoxel", false, 0, nullptr));
    return;
}
