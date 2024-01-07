// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/DirtVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UDirtVoxel::Initialize(class UGI_Master* GI)
{
    GI->AddVoxelMask(FVoxelMask("JAFG", "DirtVoxel", false, 0, nullptr));
    return;
}
