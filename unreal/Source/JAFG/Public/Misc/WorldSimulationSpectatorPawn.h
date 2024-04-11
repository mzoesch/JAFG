// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"

#include "WorldSimulationSpectatorPawn.generated.h"

class UInputComponent;
class ULocalChunkValidator;

UCLASS()
class JAFG_API AWorldSimulationSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()

public:

    explicit AWorldSimulationSpectatorPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ULocalChunkValidator> LocalChunkValidator;
};
