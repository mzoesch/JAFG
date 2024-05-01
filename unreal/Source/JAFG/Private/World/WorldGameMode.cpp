// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGameMode.h"

#include "Player/World/WorldPlayerController.h"
#include "World/WorldCharacter.h"
#include "World/WorldHUD.h"

AWorldGameMode::AWorldGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HUDClass = AWorldHUD::StaticClass();
    this->PlayerControllerClass = AWorldPlayerController::StaticClass();

    // Remove later and determine at runtime!
    this->DefaultPawnClass = AWorldCharacter::StaticClass();
}
