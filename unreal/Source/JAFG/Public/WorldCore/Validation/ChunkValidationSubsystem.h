// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkValidationSubsystem.generated.h"

class UChunkGenerationSubsystem;
JAFG_VOID

/**
 * Handles the validation of chunks.
 * Based on the local player position and chunk world generation distance.
 * This subsystem is therefore never created on a dedicated server.
 *
 * Note that the chunks loaded here (on the client or standalone) are different from the chunks loaded on the server.
 * The server will always only load the surrounding chunks of the players, while the client can load any chunk it
 * wants. See UChunkValidationSubsystemDedSv for the server side.
 */
UCLASS()
class JAFG_API UChunkValidationSubsystem final : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkValidationSubsystem();

    // WorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
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

    static auto GetAllChunksInDistance(const FChunkKey2& Center, const int32 Distance) -> TArray<FChunkKey2>;

#if !UE_BUILD_SHIPPING
    int32      MockCursor           = 1;
    int32      MockCurrentMoveIndex = 0;
    FChunkKey2 MockTargetPoint      = FChunkKey2(0, 0);
    int32      MockTimesToMove      = 1;

    bool       bFinishedMockingChunkGeneration = false;

    void CreateMockChunks(void);
#endif /* !UE_BUILD_SHIPPING */

};
