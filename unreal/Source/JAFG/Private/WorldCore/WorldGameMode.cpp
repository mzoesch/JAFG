// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldGameMode.h"

#include "JAFG/Public/Player/WorldPlayerController.h"
#include "JAFG/Public/WorldCore/WorldCharacter.h"
#include "UI/WorldHUD.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/WorldGameSession.h"
#include "WorldCore/WorldPlayerState.h"

AWorldGameMode::AWorldGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->GameSessionClass      = AWorldGameSession::StaticClass();

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
    check( WorldSettings )

    UClass* LastResort = WorldSettings->CharacterToUse;
    check( LastResort )

    return LastResort;
}

void AWorldGameMode::StartPlay(void)
{
    Super::StartPlay();

    const AEditorChunkWorldSettings* WorldSettings = Cast<AEditorChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
    check( WorldSettings )

    this->DefaultPawnClass = WorldSettings->CharacterToUse;
    check( this->DefaultPawnClass )

    return;
}

void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    NewPlayer->GetPlayerState<AWorldPlayerState>()->SetPlayerDisplayName(NewPlayer->GetName());

    return;
}
