// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/SpectatorPawn.h"

#include "WorldSimulationSpectatorPawn.generated.h"

JAFG_VOID

class ULocalChunkValidator;
struct FInputActionValue;

UCLASS(NotBlueprintable)
class JAFG_API AWorldSimulationSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()

public:

    explicit AWorldSimulationSpectatorPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

    IConsoleCommand* CommandToggleDebugScreen = nullptr;
    void OnToggleDebugScreen(const FInputActionValue& Value);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ULocalChunkValidator> LocalChunkValidator;
};
