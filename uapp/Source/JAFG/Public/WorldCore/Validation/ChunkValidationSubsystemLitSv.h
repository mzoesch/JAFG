// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/Validation/CommonValidation.h"

#include "ChunkValidationSubsystemLitSv.generated.h"

JAFG_VOID

/**
 * Handles the validation of chunks.
 * Based on the local player location, the connected clients locations and chunk world generation distance.
 * This subsystem is therefore only being created if the game is running in a listen server mode.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkValidationSubsystemLitSv : public UChunkValidationSubsystemCommon
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemLitSv();

    // WorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation
};
