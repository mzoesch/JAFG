// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/CraftingTableVoxel.h"

#include "Core/CH_Master.h"
#include "World/VoxelMask.h"
#include "Core/GI_Master.h"
#include "Core/PC_Master.h"
#include "HUD/HUD_Master.h"
#include "HUD/Container/PlayerInventory.h"
#include "Lib/DerivedClassHolder.h"

#pragma region Graphical User Interface

void UW_CraftingTableContainer::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    this->MarkAsDirty();
    return;
}

void UW_CraftingTableContainer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    bool bDiscardTick = false;
    Super::NativeTickImpl(MyGeometry, InDeltaTime, bDiscardTick);
    if (bDiscardTick == true)
    {
        return;
    }

    this->RefreshCharacterInventory();
}

void UW_CraftingTableContainer::RefreshCharacterInventory()
{
    Super::RefreshCharacterInventory();

    /* TODO Here than update all crafter slots ect. */
    
    return;
}

#pragma endregion Graphical User Interface

#pragma region Voxel

void UCraftingTableVoxel::Initialize(UGI_Master* InGIPtr)
{
    this->GIPtr = InGIPtr;

    TScriptInterface<IVoxel> VoxelPtr = TScriptInterface<IVoxel>(this);
    this->GIPtr->AddVoxelMask(FVoxelMask("JAFG", "CraftingTableVoxel", false, 0, &VoxelPtr));
    
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
    Cast<AHUD_Master>(Caller->GetWorld()->GetFirstPlayerController()->GetHUD())->AddContainer(Ident, this->GIPtr->DerivedClassHolder->CraftingTableContainer);

    /*
     * This is maybe unnecessary but for whatever reason the next secondary input action by the user is always
     * discarded by the engine itself?? This also does not fix the issue. Can be removed but is maybe a good
     * first idea?
     */
    this->GIPtr->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, Caller, Ident]()
    {
        Cast<APC_Master>(Caller->GetWorld()->GetFirstPlayerController())->TransitToContainerState(Ident, true);
    }));
    
    return;
}

#pragma endregion Voxel
