// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/GrassVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UGrassVoxel::Initialize(UGI_Master* GIPtr)
{
    TMap<ENormalLookup, int> NormalLookup = TMap<ENormalLookup, int>();
    NormalLookup.Add(ENormalLookup::NL_Top, 1);
    
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "GrassVoxel", NormalLookup, nullptr));
    return;
}

void UGrassVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    UE_LOG(LogTemp, Fatal, TEXT("UGrassVoxel::OnCustomSecondaryCharacterEvent"))
}
