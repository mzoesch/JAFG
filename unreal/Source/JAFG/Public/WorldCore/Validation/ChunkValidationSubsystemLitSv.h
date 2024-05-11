// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkValidationSubsystemLitSv.generated.h"

JAFG_VOID

class UChunkGenerationSubsystem;

/**
 * Handles the validation of chunks.
 * Based on the local player location, the connected clients locations and chunk world generation distance.
 * This subsystem is therefore only being created if the game is running in a listen server mode.
 */
UCLASS()
class JAFG_API UChunkValidationSubsystemLitSv : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemLitSv();

    // WorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    // FTickableGameObject implementation
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkValidationSubsystem, STATGROUP_Tickables);
    }
    // ~FTickableGameObject implementation

    // UJAFGTickableWorldSubsystem implementation
    virtual auto MyTick(const float DeltaTime) -> void override;
    // ~UJAFGTickableWorldSubsystem implementation

private:

    /** Copied for faster access. */
    UPROPERTY()
    TObjectPtr<UChunkGenerationSubsystem> ChunkGenerationSubsystem;

    /**
     * Will load and unload the chunks from the local player's perspective, meaning the chunks from the host
     * of this currently running listen server.
     */
    auto LoadUnLoadMyChunks(const FVector& LocalPlayerLocation) const -> void;
    /**
     * Will load and unload the chunks from the perspective of the other players, meaning the chunks from the
     * clients connected to this currently running listen server.
     */
    auto LoadUnloadTheirChunks(void) const -> void;
};
