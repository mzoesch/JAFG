// © 2023 mzoesch. All rights reserved.


#include "Core/HUD_Master.h"

#include "Blueprint/UserWidget.h"

#include "HUD/UW_Master.h"
#include "HUD/UW_Hotbar.h"
#include "HUD/UW_HotbarSlot.h"

AHUD_Master::AHUD_Master()
{
    // Set this character to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    return;
}

void AHUD_Master::BeginPlay()
{
    Super::BeginPlay();

    check(this->UWCrosshairClass)
    this->UWCrosshair = CreateWidget<UUW_Master>(this->GetWorld(), this->UWCrosshairClass);
    this->UWCrosshair->AddToViewport();

    check(this->UWHotbarClass)
    this->UWHotbar = CreateWidget<UUW_Hotbar>(this->GetWorld(), this->UWHotbarClass);
    this->UWHotbar->AddToViewport();

    check(this->UWHotbarSlotClass)
    this->UWHotbar->InitializeHotbar(this->UWHotbarSlotClass, this->UWHotbarSelectorClass);
    
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
