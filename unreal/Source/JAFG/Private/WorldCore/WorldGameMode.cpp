// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldGameMode.h"

#include "JAFG/Public/Player/WorldPlayerController.h"
#include "JAFG/Public/WorldCore/WorldCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WorldHUD.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/WorldGameSession.h"
#include "WorldCore/WorldGameState.h"
#include "WorldCore/WorldPawn.h"
#include "WorldCore/WorldPlayerState.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

AWorldGameMode::AWorldGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->GameSessionClass      = AWorldGameSession::StaticClass();
    this->GameStateClass        = AWorldGameState::StaticClass();
    this->PlayerControllerClass = AWorldPlayerController::StaticClass();
    this->PlayerStateClass      = AWorldPlayerState::StaticClass();
    this->HUDClass              = AWorldHUD::StaticClass();
    this->DefaultPawnClass      = nullptr;

    return;
}

void AWorldGameMode::BeginPlay(void)
{
    Super::BeginPlay();
}

UClass* AWorldGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (UClass* Pawn = Super::GetDefaultPawnClassForController_Implementation(InController))
    {
        return Pawn;
    }

    const AEditorChunkWorldSettings* WorldSettings = Cast<AEditorChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
    jcheck( WorldSettings )

    UClass* LastResort = WorldSettings->PendingPawn;
    jcheck( LastResort )

    return LastResort;
}

void AWorldGameMode::StartPlay(void)
{
    Super::StartPlay();

    const AEditorChunkWorldSettings* WorldSettings = Cast<AEditorChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
    jcheck( WorldSettings )

    this->DefaultPawnClass = WorldSettings->PendingPawn;
    jcheck( this->DefaultPawnClass )

    return;
}

void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AWorldGameMode::SpawnCharacterForPlayer(AWorldPlayerController* Target)
{
    jcheck( Target )
    jcheck( Target->GetPawn() )

    if (Target->GetPawn()->IsA<AWorldPawn>() == false)
    {
        LOG_FATAL(LogWorldGameMode, "Player [%s] has an invalid pawn for spawning.", *Target->GetDisplayName())
        return;
    }

    FVector PredictedLocation;
    if (Target->GetPredictedCharacterLocation(PredictedLocation) == false)
    {
        LOG_FATAL(LogWorldGameMode, "Does not know where to spawn character for player [%s] as we can not predict their location.", *Target->GetDisplayName())
        return;
    }

    if (Target->GetPawn()->Destroy() == false)
    {
        LOG_FATAL(LogWorldGameMode, "Failed to destroy pending pawn for player [%s].", *Target->GetDisplayName())
        return;
    }

    const AEditorChunkWorldSettings* WorldSettings = Cast<AEditorChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
    jcheck( WorldSettings ) jcheck( WorldSettings->CharacterToUse )

    FVector SpawnPoint;
    if (this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>()->FindAppropriateLocationForCharacterSpawn(PredictedLocation, SpawnPoint) == false)
    {
        LOG_FATAL(LogWorldGameMode, "Failed to find appropriate spawn location for player [%s].", *Target->GetDisplayName())
        return;
    }

    LOG_VERBOSE(LogWorldGameMode, "Spawning character for player [%s] at location [%s].", *Target->GetDisplayName(), *SpawnPoint.ToString())

    AWorldCharacter* Result = GetWorld()->SpawnActorDeferred<AWorldCharacter>(
        WorldSettings->CharacterToUse,
        FTransform(SpawnPoint),
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
    );
    if (Result == nullptr)
    {
        LOG_FATAL(LogWorldGameMode, "Failed to spawn character for player [%s].", *Target->GetDisplayName())
        return;
    }

    Target->SetPawn(Result);
    Target->Possess(Target->GetPawn());

    Target->ClientSetRotation(Target->GetPawn()->GetActorRotation(), true);
    Target->SetControlRotation(FRotator::ZeroRotator);

    this->SetPlayerDefaults(Target->GetPawn());

    this->K2_OnRestartPlayer(Target);

    UGameplayStatics::FinishSpawningActor(Result, FTransform(SpawnPoint));

    return;
}
