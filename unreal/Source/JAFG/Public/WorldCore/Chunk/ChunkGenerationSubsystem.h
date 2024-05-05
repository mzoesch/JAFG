// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ChunkGenerationSubsystem.generated.h"

JAFG_VOID

class ULocalChunkWorldSettings;

/**
 * Loads and unloads chunk into / from the UWorld based on what the validation subsystem has determined.
 */
UCLASS(NotBlueprintable)
class JAFG_API UChunkGenerationSubsystem final : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UChunkGenerationSubsystem();

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    // ~UWorldSubsystem implementation

private:

    UPROPERTY()
    TObjectPtr<ULocalChunkWorldSettings> LocalChunkWorldSettings;

    void SpawnChunk(const FChunkKey& ChunkKey);
    // Only temporary
    friend class UChunkValidationSubsystem;
};
