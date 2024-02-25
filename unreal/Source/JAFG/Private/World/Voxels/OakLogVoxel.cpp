// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/OakLogVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UOakLogVoxel::Initialize(UGI_Master* GIPtr)
{
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "OakLogVoxel", false, 0, nullptr));
    return;
}

void UOakLogVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    UE_LOG(LogTemp, Fatal, TEXT("UOakLogVoxel::OnCustomSecondaryCharacterEvent"))
}
