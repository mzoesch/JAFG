// Copyright 2024 mzoesch. All rights reserved.


#include "Accessors/WorldSimulationSpectatorPawn.h"


// Sets default values
AWorldSimulationSpectatorPawn::AWorldSimulationSpectatorPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWorldSimulationSpectatorPawn::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AWorldSimulationSpectatorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWorldSimulationSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

