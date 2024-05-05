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
