// © 2023 mzoesch. All rights reserved.

#include "World/Voxels/CraftingTableVoxel.h"

#include "Components/TileView.h"

#include "Core/CH_Master.h"
#include "World/VoxelMask.h"
#include "Core/GI_Master.h"
#include "Core/GM_Master.h"
#include "Core/PC_Master.h"
#include "HUD/HUD_Master.h"
#include "HUD/Container/PlayerInventory.h"
#include "Lib/DerivedClassHolder.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define OUTER_CRAFTER                       Cast<UW_CraftingTableContainer>(this->ContainerSlotData->OuterContainer)
#define CHARACTER                           Cast<ACH_Master>(this->GetOwningPlayerPawn())
#define GAME_MODE                           Cast<AGM_Master>(this->GetGameInstance()->GetWorld()->GetAuthGameMode())

#pragma region Graphical User Interface

void UW_CraftingTableCrafterSlot::OnClicked()
{
    bool bContentsChanged = false;
    OUTER_CRAFTER->CrafterSlots[this->ContainerSlotData->Index].OnClicked(CHARACTER, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    this->ContainerSlotData->OuterContainer->MarkAsDirty();
    
    return;
}

void UW_CraftingTableCrafterSlot::OnSecondaryClicked()
{
    bool bContentsChanged = false;
    OUTER_CRAFTER->CrafterSlots[this->ContainerSlotData->Index].OnSecondaryClicked(CHARACTER, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    this->ContainerSlotData->OuterContainer->MarkAsDirty();
    
    return;
}

void UW_CraftingTableCrafterProduct::OnClicked()
{
    const FAccumulated Product = OUTER_CRAFTER->GetCrafterProduct();

    if (Product == FAccumulated::NullAccumulated)
    {
        return;
    }

    if (CHARACTER->CursorHand != FAccumulated::NullAccumulated)
    {
        if (CHARACTER->CursorHand == Product)
        {
            OUTER_CRAFTER->ReduceCrafterByProductIngredients();
            CHARACTER->CursorHand.Amount += Product.Amount;

            this->ContainerSlotData->OuterContainer->MarkAsDirty();

            return;
        }

        return;
    }

    OUTER_CRAFTER->ReduceCrafterByProductIngredients();
    CHARACTER->CursorHand = Product;

    this->ContainerSlotData->OuterContainer->MarkAsDirty();
    
    return;
}

void UW_CraftingTableContainer::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->CrafterSlots.Empty();
    this->CrafterSlots.SetNum(UCraftingTableVoxel::CrafterSlotCount, false);

    return;
}

void UW_CraftingTableContainer::OnDestroy()
{
    for (int i = 0; i < UCraftingTableVoxel::CrafterSlotCount; ++i)
    {
        this->CrafterSlots[i].OnDestroy(CHARACTER);
        continue;
    }

    return;
}

void UW_CraftingTableContainer::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
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

    this->CraftingTableCrafterSlots->ClearListItems();

    for (int i = 0; i < UCraftingTableVoxel::CrafterSlotCount; ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);

        Data->Index          = i;
        Data->Accumulated    = this->CrafterSlots[i].Content;
        Data->OuterContainer = this;

        this->CraftingTableCrafterSlots->AddItem(Data);

        continue;
    }

    UContainerSlotData* CrafterProductData  = NewObject<UContainerSlotData>(this);
    CrafterProductData->Index               = 0;
    CrafterProductData->Accumulated         = this->GetCrafterProduct();
    CrafterProductData->OuterContainer      = this;
    this->CraftingTableCrafterProduct->ContainerSlotData = CrafterProductData;
    this->CraftingTableCrafterProduct->RenderSlot();
    
    return;
}

FDelivery UW_CraftingTableContainer::GetCrafterAsDelivery()
{
    TArray<FAccumulated> DeliveryContents;
    for (const FSlot& CrafterSlot : this->CrafterSlots)
    {
        DeliveryContents.Add(FAccumulated(CrafterSlot.Content.Accumulated));
        continue;
    }

    return FDelivery { DeliveryContents, UCraftingTableVoxel::CrafterWidth };
}

FAccumulated UW_CraftingTableContainer::GetCrafterProduct()
{
    FAccumulated Product;
    GAME_MODE->PrescriptionSeeker->GetProduct(this->GetCrafterAsDelivery(), Product);

    return Product;
}

void UW_CraftingTableContainer::ReduceCrafterByProductIngredients()
{
    for (int i = 0; i < UCraftingTableVoxel::CrafterSlotCount; ++i)
    {
        if (this->CrafterSlots[i].Content == FAccumulated::NullAccumulated)
        {
            continue;
        }

        bool bSuccess;
        this->CrafterSlots[i].Content.SafeAddAmount(-1, bSuccess);
        if (bSuccess == false)
        {
            UIL_LOG(
                Fatal,
                TEXT("UW_CraftingTableContainer::ReduceCrafterByProductIngredients: Failed to safe reduce the amount of the accumulated item in the crafting table crafter for %d slot. Found %s."),
                i, *this->CrafterSlots[i].Content.ToString()
            );
            return;
        }
        
        continue;
    }

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
    // UW_CraftingTableContainer* NewWidget = Cast<UW_CraftingTableContainer>(Cast<AHUD_Master>(Caller->GetWorld()->GetFirstPlayerController()->GetHUD())->GetContainer(Ident));
    
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

#undef UIL_LOG
#undef OUTER_CRAFTER
#undef CHARACTER
#undef GAME_MODE
