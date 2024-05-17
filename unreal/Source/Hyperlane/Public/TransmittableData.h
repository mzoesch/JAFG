// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "System/TypeDefs.h"
#include "Serialization/BufferArchive.h"
#include "WorldCore/WorldTypes.h"

namespace TransmittableData
{

/** The type of message that is being transmitted. Marked as a byte-field and must always be prefixed to the message. */
typedef uint8 DataTransmissionType;

namespace EDataTransmissionType
{

enum Type : DataTransmissionType
{
    Invalid                 = 0b00,
    Ping                    = 0b01,
    ChunkInitializationData = 0b10,
};

}

FORCEINLINE static int32 GetTypeSize(const EDataTransmissionType::Type InType)
{
    switch (InType)
    {
    case EDataTransmissionType::Invalid:
    {
        LOG_FATAL(LogHyperlane, "Queried for the invalid state.")
        return 0;
    }
    case EDataTransmissionType::Ping:
    {
        return sizeof(DataTransmissionType);
    }
    case EDataTransmissionType::ChunkInitializationData:
    {
        return sizeof(DataTransmissionType) + sizeof(FChunkKey) + sizeof(voxel_t) * WorldStatics::VoxelCount;
    }
    default:
    {
        checkNoEntry()
        return 0;
    }
    }
}

FORCEINLINE static auto SerializeType(FArchive& Ar, const EDataTransmissionType::Type& TransmissionType) -> void
{
    DataTransmissionType AsByte = TransmissionType; Ar << AsByte;
}

FORCEINLINE static auto DeserializeType(FArchive& Ar, EDataTransmissionType::Type& TransmissionType) -> void
{
    DataTransmissionType FirstByte; Ar << FirstByte;
    TransmissionType = static_cast<EDataTransmissionType::Type>(FirstByte);
}

/**
 * @param Bytes Recv buffer of the socket connection.
 * @return The type of the message that was transmitted.
 */
FORCEINLINE static EDataTransmissionType::Type DeserializeType(const TArray<uint8>& Bytes)
{
    FMemoryReader Ar = FMemoryReader(Bytes, true);
    Ar.Seek(0b0);

    EDataTransmissionType::Type Type; TransmittableData::DeserializeType(Ar, Type);

    Ar.FlushCache();
    if (Ar.Close() == false)
    {
        LOG_FATAL(LogHyperlane, "Failed to close memory reader.")
    }

    return Type;
}

FORCEINLINE static auto GetPingAsBytes(void) -> TArray<uint8>
{
    TArray<uint8> Bytes;
    FMemoryWriter Ar = FMemoryWriter(Bytes, true);
    Ar.Seek(0b0);

    TransmittableData::SerializeType(Ar, EDataTransmissionType::Ping);

    Ar.FlushCache();
    if (Ar.Close() == false)
    {
        LOG_FATAL(LogHyperlane, "Failed to close memory writer.")
    }

    return Bytes;
}

struct FChunkInitializationData
{
    FChunkKey ChunkKey;
    /** A dynamic array with the size of WorldStatics::VoxelCount. */
    voxel_t*  Voxels;

    FORCEINLINE auto SerializeToBytes(TArray<uint8>& OutBytes) -> void
    {
        FBufferArchive Ar = FBufferArchive();
        Ar.Seek(0b0);

        Ar << this;

        OutBytes.Reset(Ar.Num());
        OutBytes.Append(Ar.GetData(), Ar.Num());

        check(
            OutBytes.Num() <=
            /* The predicted size. Hardcoded here, so we do not accidentally change struct sizes. */
            16397
        )

        Ar.FlushCache();
        Ar.Empty();

        return;
    }

    /** Only use for serialization. */
    FORCEINLINE friend auto operator<<(FArchive& Ar, FChunkInitializationData* Data) -> FArchive&
    {
        TransmittableData::SerializeType(Ar, EDataTransmissionType::ChunkInitializationData);
        Ar << Data->ChunkKey;
        for (int32 i = 0; i < WorldStatics::VoxelCount; ++i)
        {
            Ar << Data->Voxels[i];
        }
        return Ar;
    }

    /** Only use for deserialization. */
    FORCEINLINE friend auto operator<<(FArchive& Ar, FChunkInitializationData& Data) -> FArchive&
    {
        /* We can ignore the type. It must have been already identified. */
        uint8 Type; Ar << Type;
        Ar << Data.ChunkKey;
        for (int32 i = 0; i < WorldStatics::VoxelCount; ++i)
        {
            Ar << Data.Voxels[i];
        }
        return Ar;
    }
};

FORCEINLINE auto DeserializeChunkInitializationData(const TArray<uint8>& InBytes) -> TransmittableData::FChunkInitializationData
{
    TransmittableData::FChunkInitializationData Data;
    Data.Voxels = new voxel_t[WorldStatics::VoxelCount];

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
