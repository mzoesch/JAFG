// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGameMode.h"

#include "Misc/WorldSimulationSpectatorPawn.h"
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
    this->HUDClass              = AWorldHUD::StaticClass();
    this->DefaultPawnClass      = nullptr; /* Determined at runtime. */
    this->SpectatorClass        = AWorldSimulationSpectatorPawn::StaticClass();
    /* ReplaySpectatorClass */
    /* ServerStatReplicatorClass */

    return;
}

void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    check( NewPlayer )

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(NewPlayer);
    if (WorldPlayerController == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogWorldGameMode, "Player %s is not of type AWorldPlayerController. Discarding post login event.", *NewPlayer->GetName());
#else /* WITH_EDITOR */
        LOG_FATAL(LogWorldGameMode, "Player %s is not of type AWorldPlayerController.", *NewPlayer->GetName());
#endif /* !WITH_EDITOR */
        return;
    }

    WorldPlayerController->OnPostLogin();

    UE_LOG(LogTemp, Warning, TEXT("Player %s has joined the game."), *NewPlayer->GetName());

    return;
}
