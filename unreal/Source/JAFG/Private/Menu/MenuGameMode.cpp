// Copyright 2024 mzoesch. All rights reserved.

#include "Menu/MenuGameMode.h"

#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/HUD.h"
#include "GameFramework/SpectatorPawn.h"

#include "Menu/MenuCharacter.h"
#include "Menu/MenuPlayerController.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->GameSessionClass                     = AGameSession::StaticClass();
    this->GameStateClass                       = AGameStateBase::StaticClass();
    this->PlayerControllerClass                = AMenuPlayerController::StaticClass();
    this->PlayerStateClass                     = APlayerState::StaticClass();
    this->HUDClass                             = AHUD::StaticClass();
    this->DefaultPawnClass                     = AMenuCharacter::StaticClass();
    this->SpectatorClass                       = ASpectatorPawn::StaticClass();
    this->ReplaySpectatorPlayerControllerClass = APlayerController::StaticClass();
    this->ServerStatReplicatorClass            = AServerStatReplicator::StaticClass();

    return;
}
