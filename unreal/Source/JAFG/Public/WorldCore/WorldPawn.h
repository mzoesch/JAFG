// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/Pawn.h"

#include "WorldPawn.generated.h"

JAFG_VOID

/**
 * A pawn that can be possessed by a player controller for only a very short amount of time.
 * Is used to overcome loading spans where a controlled character is not yet available but absolutely necessary.
 * The pawn has no physical representation in the world and will result in a client crash if it exists for too long.
 */
UCLASS(NotBlueprintable)
class JAFG_API AWorldPawn : public APawn
{
    GENERATED_BODY()

public:

    explicit AWorldPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(float DeltaTime) override;

private:

    static constexpr float Timeout = 10.0f;
    void TimeoutDestroy(void);
};
