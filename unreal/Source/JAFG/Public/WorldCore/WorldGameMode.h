// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "WorldGameMode.generated.h"

UCLASS()
class JAFG_API AWorldGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit AWorldGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // AActor implementation
    virtual auto BeginPlay(void) -> void override;
    // ~AActor implementation

    // AGameModeBase implementation

    //~=============================================================================
    // Accessors for classes spawned by game
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

    //~=============================================================================
    // Starting / pausing / resetting the game
    virtual auto StartPlay(void) -> void override;

protected:

    //~=============================================================================
    // Player joining and leaving

    virtual auto PostLogin(APlayerController* NewPlayer) -> void override;

public:

    //~=============================================================================
    // Spawning the player's pawn

    // ~AGameModeBase implementation
};
