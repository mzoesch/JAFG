// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/Validation/CommonValidation.h"

#include "ChunkValidationSubsystemCl.generated.h"

JAFG_VOID

class UChunkGenerationSubsystem;

/**
 * Handles the validation of chunks.
 * Based on the local player location and chunk world generation distance.
 * This subsystem is therefore only being created if the game is running as a client conneted to some form of server.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkValidationSubsystemCl : public UChunkValidationSubsystemCommon
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemCl();

    // WorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation
};
