// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/StoneVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UStoneVoxel::Initialize(UGI_Master* GIPtr)
{
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "StoneVoxel", false, 0, nullptr));
    return;
}

void UStoneVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    UE_LOG(LogTemp, Fatal, TEXT("UStoneVoxel::OnCustomSecondaryCharacterEvent"))
}
