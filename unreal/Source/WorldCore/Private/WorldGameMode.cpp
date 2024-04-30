// Copyright 2024 mzoesch. All rights reserved.

#include "WorldGameMode.h"

#include "Accessors/WorldCharacter.h"
#include "Accessors/WorldGameState.h"
#include "Accessors/WorldHUD.h"
#include "Accessors/WorldPlayerController.h"
#include "Accessors/WorldPlayerState.h"
#include "Accessors/WorldSimulationSpectatorPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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

void AWorldGameMode::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->DefaultWorldCharacterClass == nullptr)
    {
        LOG_FATAL(LogWorldGameMode, "Default World Character class is not set.")
        return;
    }

    return;
}

void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

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

    LOG_WARNING(LogWorldGameMode, "Player %s has joined the game.", *NewPlayer->GetName());

    AActor* Start = UGameplayStatics::GetActorOfClass(this->GetWorld(), APlayerStart::StaticClass());

    if (Start == nullptr)
    {
        LOG_FATAL(LogWorldGameMode, "No player start found.")
        return;
    }

    AWorldCharacter* Char = this->GetWorld()->SpawnActor<AWorldCharacter>(this->DefaultWorldCharacterClass, Start->GetTransform());

    Char->SetOwner(NewPlayer);
    NewPlayer->Possess(Char);

    return;
}
