// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkValidationSubsystemStandalone.generated.h"

JAFG_VOID

class UChunkGenerationSubsystem;

/**
 * Handles the validation of chunks.
 * Based on the local player position and chunk world generation distance.
 * The subsystem will therefore only be created if the game is running in a standalone mode.
 */
UCLASS()
class JAFG_API UChunkValidationSubsystemStandalone final : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystemStandalone();

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

    void LoadUnloadChunks(const FVector& LocalPlayerLocation) const;
};
