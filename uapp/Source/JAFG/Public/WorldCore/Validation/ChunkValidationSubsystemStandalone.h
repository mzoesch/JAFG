// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/Validation/CommonValidation.h"

#include "ChunkValidationSubsystemStandalone.generated.h"

JAFG_VOID

/**
 * Handles the validation of chunks.
 * Based on the local player position and chunk world generation distance.
 * The subsystem will therefore only be created if the game is running in a standalone mode. Note that this includes
 * the editor simulation mode as well.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkValidationSubsystemStandalone final : public UChunkValidationSubsystemCommon
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemStandalone();

    // WorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation
};
