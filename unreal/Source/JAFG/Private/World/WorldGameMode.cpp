// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGameMode.h"

#include "UI/World/WorldHUD.h"
#include "World/WorldGameState.h"
#include "World/WorldPlayerController.h"
#include "World/WorldPlayerState.h"

AWorldGameMode::AWorldGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    /* GameSessionClass */
    this->GameStateClass        = AWorldGameState::StaticClass();
    this->PlayerControllerClass = AWorldPlayerController::StaticClass();
    this->PlayerStateClass      = AWorldPlayerState::StaticClass();
    this->PlayerControllerClass = AWorldPlayerController::StaticClass();
    this->HUDClass              = AWorldHUD::StaticClass();
    this->DefaultPawnClass      = nullptr; /* Determined at runtime. */
    /* SpectatorClass */
    /* ReplaySpectatorClass */
    /* ServerStatReplicatorClass */

    return;
}
