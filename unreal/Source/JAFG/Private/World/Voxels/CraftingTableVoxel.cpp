// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/CraftingTableVoxel.h"

#include "World/VoxelMask.h"
#include "Core/GI_Master.h"

void UCraftingTableVoxel::Initialize(UGI_Master* GIPtr)
{
    TScriptInterface<IVoxel> VoxelPtr = TScriptInterface<IVoxel>(this);
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "CraftingTableVoxel", false, 0, &VoxelPtr));

    // We need to set some addition vars here.
    // Some examples
    // Some special block stuff. Like their effects on the UWorld
    // bHasCustomSecondaryEvent
    // ...
    // We set all this vars make them public and write a gateway in the voxel-mask
    
    return;
}

void UCraftingTableVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    UE_LOG(LogTemp, Error, TEXT("UCraftingTableVoxel::OnCustomSecondaryCharacterEvent"))
    bConsumed = true;
}
