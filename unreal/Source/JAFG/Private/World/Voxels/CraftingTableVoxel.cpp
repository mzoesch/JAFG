// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/CraftingTableVoxel.h"

#include "Core/CH_Master.h"
#include "World/VoxelMask.h"
#include "Core/GI_Master.h"
#include "Core/PC_Master.h"
#include "HUD/HUD_Master.h"
#include "HUD/UW_Master.h"
#include "HUD/Container/PlayerInventory.h"

void UCraftingTableVoxel::Initialize(UGI_Master* GIPtr)
{
    TScriptInterface<IVoxel> VoxelPtr = TScriptInterface<IVoxel>(this);
    GIPtr->AddVoxelMask(FVoxelMask("JAFG", "CraftingTableVoxel", false, 0, &VoxelPtr));
    
    // We need to set some additional vars here.
    // Some examples
    // Some special block stuff. Like their effects on the UWorld
    // bHasCustomSecondaryEvent
    // ...
    // We set all these vars, make them public and write a gateway in the FVoxelMask.
    
    return;
}

void UCraftingTableVoxel::OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed)
{
    bConsumed = true;

    FString Ident;
    Cast<AHUD_Master>(Caller->GetWorld()->GetFirstPlayerController()->GetHUD())->AddContainer(Ident,
        Cast<AHUD_Master>(Caller->GetWorld()->GetFirstPlayerController()->GetHUD())->UWPlayerInventoryClass);

    UE_LOG(LogTemp, Error, TEXT("UCraftingTableVoxel::OnCustomSecondaryCharacterEvent: %s."), *Ident)

    Cast<APC_Master>(Caller->GetWorld()->GetFirstPlayerController())->TransitToContainerState(Ident, true);
    
    return;
}
