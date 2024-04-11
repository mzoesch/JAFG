// Copyright 2024 mzoesch. All rights reserved.

#include "Misc/WorldSimulationSpectatorPawn.h"

#include "World/Chunk/LocalChunkValidator.h"

AWorldSimulationSpectatorPawn::AWorldSimulationSpectatorPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->LocalChunkValidator = CreateDefaultSubobject<ULocalChunkValidator>(TEXT("LocalChunkValidator"));

    return;
}

void AWorldSimulationSpectatorPawn::BeginPlay(void)
{
    Super::BeginPlay();
}

void AWorldSimulationSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
