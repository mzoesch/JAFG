// © 2023 mzoesch. All rights reserved.


#include "HUD/HUD_Master.h"

#include "Blueprint/UserWidget.h"

#include "HUD/UW_Master.h"
#include "HUD/UW_Hotbar.h"
#include "HUD/UW_HotbarSlot.h"
#include "HUD/UW_Inventory.h"
#include "Core/CH_Master.h"

AHUD_Master::AHUD_Master()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->UWCrosshair = nullptr;
    this->UWHotbar = nullptr;
    
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
    this->UWCrosshair = CreateWidget<UUW_Master>(this->GetWorld(), this->UWCrosshairClass);
    this->UWCrosshair->AddToViewport();

    check(this->UWHotbarClass)
    this->UWHotbar = CreateWidget<UUW_Hotbar>(this->GetWorld(), this->UWHotbarClass);
    this->UWHotbar->AddToViewport();

    check(this->UWHotbarSlotClass)
    this->UWHotbar->InitializeHotbar(this->UWHotbarSlotClass, this->UWHotbarSelectorClass);

    check(this->UWInventoryClass)
    this->UWInventory = CreateWidget<UUW_Inventory>(this->GetWorld(), this->UWInventoryClass);
    this->UWInventory->AddToViewport();
    this->UWInventory->SetVisibility(ESlateVisibility::Hidden);
    
    return;
}

void AHUD_Master::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UE_LOG(LogTemp, Error, TEXT("HUD_Master::Tick()"))
    return;
}

void AHUD_Master::OnQuickSlotSelect() const
{
    this->UWHotbar->OnSlotSelect();
    return;
}

void AHUD_Master::OnHotbarUpdate()
{
    this->UWHotbar->OnHotbarUpdate();
}

void AHUD_Master::OnInventoryToggle(const bool bOpen) const
{
    if (bOpen == false)
    {
        this->UWInventory->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    this->UWInventory->SetVisibility(ESlateVisibility::Visible);

    return;
}
