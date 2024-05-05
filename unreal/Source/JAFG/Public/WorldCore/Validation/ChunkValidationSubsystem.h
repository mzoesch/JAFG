// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkValidationSubsystem.generated.h"

JAFG_VOID

/**
 * Handles the validation of chunks.
 * Based on the local player position and chunk world generation distance.
 *
 * Note that the chunks loaded here (on the client or standalone) are different from the chunks loaded on the server.
 * The server will always only load the surrounding chunks of the players, while the client can load any chunk it
 * wants. See UChunkValidationSubsystemDedSv for the server side.
 */
UCLASS()
class JAFG_API UChunkValidationSubsystem : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystem();

    // WorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    // ~WorldSubsystem implementation

    // FTickableGameObject implementation
                virtual auto Tick(const float DeltaTime) -> void override;
    FORCEINLINE virtual auto GetStatId(void) const -> TStatId override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UChunkValidationSubsystem, STATGROUP_Tickables);
    }
    // ~FTickableGameObject implementation

private:

    void CreateMockChunks(void);
};
