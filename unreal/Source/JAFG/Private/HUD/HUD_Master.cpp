// © 2023 mzoesch. All rights reserved.


#include "HUD/HUD_Master.h"

#include "Blueprint/UserWidget.h"

#include "HUD/UW_Master.h"
#include "HUD/Container/Slots/HotbarSlot.h"
#include "HUD/Container/HotbarSelector.h"
#include "HUD/Container/PlayerInventory.h"
#include "HUD/OSD/DebugScreen.h"
#include "HUD/Assets/AccumulatedCursorPreview.h"
#include "Core/CH_Master.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HUD/Container/Slots/HotbarSlot.h"
#include "HUD/Container/Hotbar.h"

#define CH Cast<ACH_Master>(this->GetOwningPawn())

AHUD_Master::AHUD_Master()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->UWCrosshair = nullptr;
    this->UWHotbar = nullptr;

    this->AccumulatedCursorPreview = nullptr;
    
    return;
}

void AHUD_Master::BeginPlay()
{
    Super::BeginPlay();

    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(ACH_Master::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Error, TEXT("UUW_Hotbar::InitializeHotbar() - Owning pawn is not a valid type."));
        return;
    }
    
    check(this->UWCrosshairClass)
    this->UWCrosshair = CreateWidget<UW_Master>(this->GetWorld(), this->UWCrosshairClass);
    this->UWCrosshair->AddToViewport();

    check(this->UWHotbarClass)
    this->UWHotbar = CreateWidget<UW_Hotbar>(this->GetWorld(), this->UWHotbarClass);
    this->UWHotbar->AddToViewport();

    check(this->UWHotbarSlotClass)
    this->UWHotbar->InitializeHotbar(this->UWHotbarSlotClass, this->UWHotbarSelectorClass);
    
    check(this->UWPlayerInventoryClass)
    check(this->UWAccumulatedCursorPreviewClass)
    this->UWPlayerInventory = CreateWidget<UW_PlayerInventory>(this->GetWorld(), this->UWPlayerInventoryClass);
    this->UWPlayerInventory->AddToViewport();
    this->UWPlayerInventory->SetVisibility(ESlateVisibility::Hidden);
    
    return;
}

void AHUD_Master::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UE_LOG(LogTemp, Error, TEXT("HUD_Master::Tick()"))
    return;
}

void AHUD_Master::UpdateInventoryAndHotbar()
{
    if (this->AccumulatedCursorPreview != nullptr)
    {
        this->AccumulatedCursorPreview->RemoveFromParent();
        this->AccumulatedCursorPreview = nullptr;
    }

    if (CH->GetAccumulatedInCursorHand() != FAccumulated::NullAccumulated)
    {
        this->AccumulatedCursorPreview = Cast<UW_AccumulatedCursorPreview>(CreateWidget(this->GetWorld(), this->UWAccumulatedCursorPreviewClass));
        this->AccumulatedCursorPreview->AddToViewport();
    }
    
    this->UWHotbar->OnHotbarUpdate();
    this->UWPlayerInventory->OnInventoryUpdate();

    return;
}

void AHUD_Master::OnInventorySlotSelect()
{
    if (this->AccumulatedCursorPreview != nullptr)
    {
        this->AccumulatedCursorPreview->RemoveFromParent();
        this->AccumulatedCursorPreview = nullptr;
    }
    
    if (CH->GetAccumulatedInCursorHand() == FAccumulated::NullAccumulated)
    {
        this->UWPlayerInventory->OnInventoryUpdate();
        return;
    }
    
    this->AccumulatedCursorPreview = Cast<UW_AccumulatedCursorPreview>(CreateWidget(this->GetWorld(), this->UWAccumulatedCursorPreviewClass));
    this->AccumulatedCursorPreview->AddToViewport();
    
    this->UWPlayerInventory->OnInventoryUpdate();

    return;
}

void AHUD_Master::OnQuickSlotSelect() const
{
    this->UWHotbar->OnSlotSelect();
    return;
}

void AHUD_Master::OnHotbarUpdate() const
{
    this->UWHotbar->OnHotbarUpdate();
}

void AHUD_Master::OnInventoryToggle(const bool bOpen)
{
    if (bOpen == false)
    {
        this->UWPlayerInventory->SetVisibility(ESlateVisibility::Hidden);

        if (this->AccumulatedCursorPreview != nullptr)
        {
            this->AccumulatedCursorPreview->RemoveFromParent();
            this->AccumulatedCursorPreview = nullptr;
        }
        
        /* Just a backup if something fishy was going on, so that the inventory state and HUD is all synced up. */
        this->OnHotbarUpdate();
        
        return;
    }

    this->UWPlayerInventory->SetVisibility(ESlateVisibility::Visible);
    this->UWPlayerInventory->OnInventoryUpdate();
    
    return;
}

void AHUD_Master::OnDebugScreenToggle()
{
    if (this->UWDebugScreen == nullptr)
    {
        check(this->UWDebugScreenClass)
        this->UWDebugScreen = CreateWidget<UW_DebugScreen>(this->GetWorld(), this->UWDebugScreenClass);
        this->UWDebugScreen->AddToViewport();

        return;
    }

    this->UWDebugScreen->RemoveFromParent();
    this->UWDebugScreen = nullptr;

    return;
}

#undef CH
