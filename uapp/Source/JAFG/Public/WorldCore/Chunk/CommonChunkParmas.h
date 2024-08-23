// Copyright 2024 mzoesch. All rights reserved.

#pragma once

class UServerChunkWorldSettings;
class UMaterialSubsystem;
class AChunkMulticasterInfo;
class UChunkGenerationSubsystem;
class UVoxelSubsystem;

/**
 * Shared params that are *not* unique to a chunk instance and should be shared across all of them in an instance
 * of a world.
 */
struct FSharedChunkParams
{
    FSharedChunkParams() :
        VoxelSubsystem(nullptr),
        ChunkGenerationSubsystem(nullptr),
        ChunkMulticasterInfo(nullptr),
        MaterialSubsystem(nullptr),
        ServerChunkWorldSettings(nullptr)
    {
    }

    UVoxelSubsystem*           VoxelSubsystem;
    UChunkGenerationSubsystem* ChunkGenerationSubsystem;
    AChunkMulticasterInfo*     ChunkMulticasterInfo;
    UMaterialSubsystem*        MaterialSubsystem;
    /** As the name suggests only valid on an authority like instance. */
    UServerChunkWorldSettings* ServerChunkWorldSettings;
};
