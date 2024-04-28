// Copyright 2024 mzoesch. All rights reserved.

#include "MenuGameMode.h"

#include "MenuHUD.h"
#include "Player/JAFGPlayerController.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PlayerControllerClass = AJAFGPlayerController::StaticClass();
    this->HUDClass              = AMenuHUD::StaticClass();

    return;
}

void AMenuGameMode::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeStandalone(this) == false)
    {
        LOG_FATAL(LogFrontEnd, "This game mode is not intended for networked play.")
        return;
    }

    AJAFGPlayerController* OwningPlayerController = Cast<AJAFGPlayerController>(this->GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(this->GetWorld()));

    if (OwningPlayerController == nullptr)
    {
        LOG_FATAL(LogFrontEnd, "Failed to retrieve owning player controller.")
        return;
    }

    OwningPlayerController->ShowMouseCursor(true);

    return;
}
