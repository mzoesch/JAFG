// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/CraftingTableVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UCraftingTableVoxel::Initialize(UGI_Master* GIPtr)
{
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "CraftingTableVoxel", false, 0, nullptr));
    return;
}
