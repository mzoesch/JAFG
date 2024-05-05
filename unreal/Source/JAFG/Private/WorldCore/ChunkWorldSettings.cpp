// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ChunkWorldSettings.h"

ULocalChunkWorldSettings::ULocalChunkWorldSettings(void) : Super()
{
    this->LocalChunkType = EChunkType::Greedy;

    return;
}
