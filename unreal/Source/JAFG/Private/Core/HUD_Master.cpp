// © 2023 mzoesch. All rights reserved.


#include "Core/HUD_Master.h"

#include "Blueprint/UserWidget.h"

#include "HUD/UW_Master.h"

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
    
    return;
}

void AHUD_Master::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // UE_LOG(LogTemp, Error, TEXT("HUD_Master::Tick()"))
    return;
}
