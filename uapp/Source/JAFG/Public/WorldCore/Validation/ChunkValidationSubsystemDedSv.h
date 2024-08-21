// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/Validation/CommonValidation.h"

#include "ChunkValidationSubsystemDedSv.generated.h"

JAFG_VOID

/**
 * Only spawned on the dedicated server.
 * Handles the validation of chunks based on all player positions.
 *
 * Note, it completely ignores the requests of chunks from remote players.
 * The chunks that are loaded here (on the dedicated server) and the chunks on the specific clients
 * are completely detached and independent of each other.
 *
 * This is so that a client can have a disgusting render distance, but the server only has to handle the surrounding
 * chunks from the players. A server cannot possibly load all loaded chunks from all players (even harder if the
 * players are spread out and each player has completely different chunks loaded).
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkValidationSubsystemDedSv : public UChunkValidationSubsystemCommon
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemDedSv();

    // WorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation
};
