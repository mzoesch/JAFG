// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWorldSubsystems.h"

#include "ChunkWorldSettings.generated.h"

JAFG_VOID

/**
 * Contains general settings for the chunk world that only apply to the local player.
 */
UCLASS()
class JAFG_API ULocalChunkWorldSettings : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    ULocalChunkWorldSettings();

    EChunkType::Type LocalChunkType;
};

/**
 * Contains settings only valid on a server or standalone client.
 */
UCLASS()
class JAFG_API UServerChunkWorldSettings : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UServerChunkWorldSettings();

    // WorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~WorldSubsystem implementation

    EWorldGenerationType::Type WorldGenerationType = EWorldGenerationType::Invalid;
    FORCEINLINE auto GetWorldGenerationType() const -> EWorldGenerationType::Type { return this->WorldGenerationType; }
};
