// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ChunkWorldSettings.h"

ULocalChunkWorldSettings::ULocalChunkWorldSettings(void) : Super()
{
    this->LocalChunkType = EChunkType::Greedy;

    return;
}

UServerChunkWorldSettings::UServerChunkWorldSettings(void) : Super()
{
    this->WorldGenerationType = EWorldGenerationType::Superflat;
    return;
}

bool UServerChunkWorldSettings::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    if (UNetStatics::IsSafeClient(Outer))
    {
        return false;
    }

    return true;
}
