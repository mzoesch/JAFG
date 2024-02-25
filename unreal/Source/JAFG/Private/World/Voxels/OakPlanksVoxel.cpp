// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/OakPlanksVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UOakPlanksVoxel::Initialize(UGI_Master* GIPtr)
{
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "OakPlanksVoxel", false, 0, nullptr));
    return;
}

void UOakPlanksVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    UE_LOG(LogTemp, Fatal, TEXT("UOakPlanksVoxel::OnCustomSecondaryCharacterEvent"))
}
