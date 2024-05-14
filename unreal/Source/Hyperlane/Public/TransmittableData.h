// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "System/TypeDefs.h"
#include "Serialization/BufferArchive.h"
#include "WorldCore/WorldTypes.h"

namespace TransmittableData
{

struct FChunkInitializationData
{
    FChunkKey ChunkKey;
    voxel_t*  Voxels;

    static constexpr int32 VoxelSize { sizeof(voxel_t) * WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize };

    FORCEINLINE auto SerializeToBytes(TArray<uint8>& OutBytes) -> void
    {
        FBufferArchive Ar = FBufferArchive();
        Ar.Seek(0b0);

        Ar << this;

        OutBytes.Reset(Ar.Num());
        OutBytes.Append(Ar.GetData(), Ar.Num());

        Ar.FlushCache();
        Ar.Empty();

        return;
    }

    FORCEINLINE friend FArchive& operator<<( FArchive& Ar, FChunkInitializationData* Data )
    {
        Ar << Data->ChunkKey;
        for (int32 i = 0; i < FChunkInitializationData::VoxelSize; i++) { Ar << Data->Voxels[i]; }
        return Ar;
    }

    FORCEINLINE friend FArchive& operator<<( FArchive& Ar, FChunkInitializationData& Data )
    {
        Ar << Data.ChunkKey;
        for (int32 i = 0; i < FChunkInitializationData::VoxelSize; i++) { Ar << Data.Voxels[i]; }
        return Ar;
    }
};

FORCEINLINE auto DeserializeChunkInitializationData(const TArray<uint8>& InBytes) -> TransmittableData::FChunkInitializationData
{
    TransmittableData::FChunkInitializationData Data;
    Data.Voxels = new voxel_t[FChunkInitializationData::VoxelSize];

    FMemoryReader Ar = FMemoryReader(InBytes, true);
    Ar.Seek(0b0);

    Ar << Data;

    Ar.FlushCache();
    if (Ar.Close() == false)
    {
        LOG_FATAL(LogHyperlane, "Failed to close memory reader.")
    }

    return Data;
}

}
