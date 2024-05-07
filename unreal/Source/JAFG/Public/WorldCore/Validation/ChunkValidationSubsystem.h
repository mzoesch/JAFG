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
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
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

#if !UE_BUILD_SHIPPING
    int32      MockCursor                      = 1;
    int32      CurrentMoveIndex                = 0;
    FChunkKey2 TargetPoint                     = FChunkKey2(0, 0);
    int32      TimesToMove                     = 1;

    bool       bFinishedMockingChunkGeneration = false;

    void CreateMockChunks(void);
#endif /* !UE_BUILD_SHIPPING */

};
