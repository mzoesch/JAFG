// Copyright 2024 mzoesch. All rights reserved.

#include "GenPrevAssets/GenPrevAssetsGameModeBase.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerState.h"
#include "GenPrevAssets/GenPrevAssetsCharacter.h"

AGenPrevAssetsGameModeBase::AGenPrevAssetsGameModeBase(const FObjectInitializer& ObjectInitializer)
{
    /* GameSessionClass */
    this->GameStateClass        = AGameStateBase::StaticClass();
    this->PlayerControllerClass = APlayerController::StaticClass();
    this->PlayerStateClass      = APlayerState::StaticClass();
    this->HUDClass              = AHUD::StaticClass();
    this->DefaultPawnClass      = AGenPrevAssetsCharacter::StaticClass();
    this->SpectatorClass        = nullptr;
    /* ReplaySpectatorClass */
    /* ServerStatReplicatorClass */
}
