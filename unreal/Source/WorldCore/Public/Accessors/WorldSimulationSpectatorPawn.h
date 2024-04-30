// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/SpectatorPawn.h"
#include "WorldSimulationSpectatorPawn.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class WORLDCORE_API AWorldSimulationSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AWorldSimulationSpectatorPawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
