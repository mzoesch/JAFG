// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"

#include "WorldTypes.generated.h"

static const FString CommonNamespace = TEXT("COMMON");

UENUM(BlueprintType)
namespace EChunkType
{

enum Type : uint8
{
    Convex,
    Naive,
    Greedy,
    Marching,
};

}

namespace EChunkType
{

FORCEINLINE FString LexToString(const EChunkType::Type ChunkType)
{
    switch (ChunkType)
    {
    case EChunkType::Greedy:
        {
            return TEXT("Greedy");
        }
    default:
        {
            checkNoEntry()
            return TEXT("Unknown");
        }
    }
}

}

UENUM(BlueprintType)
namespace EWorldGenerationType
{

enum Type : uint8
{
    Invalid,
    Default,
    Superflat,
};

}

namespace EWorldGenerationType
{

FORCEINLINE FString LexToString(const EWorldGenerationType::Type GenerationType)
{
    switch (GenerationType)
    {
    case EWorldGenerationType::Superflat:
        {
            return TEXT("Superflat");
        }
    case EWorldGenerationType::Default:
        {
            return TEXT("Default");
        }
    default:
        {
            checkNoEntry()
            return TEXT("Unknown");
        }
    }
}

}

namespace WorldStatics
{

/**
 * Do not increase this value beyond 16 for now. As we would breach the bunch size limit of 2^16 = 65.536 bytes.
 * See CommonChunk.h for more information.
 */
static constexpr int32  ChunkSize        { 16 };
static constexpr int32  ChunkSizeSquared { WorldStatics::ChunkSize * WorldStatics::ChunkSize };
static constexpr int32  ChunkSizeCubed   { WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize };
/** Per chunk basis. */
static constexpr int32  VoxelCount       { WorldStatics::ChunkSizeCubed };

static constexpr float  SingleVoxelSize            { 100.0f };
static constexpr double SingleVoxelSizeDouble      { 100.0  };
static constexpr int32  SingleVoxelSizeInteger     { 100    };
static constexpr float  SingleVoxelSizeHalf        { 50.0f  };
static constexpr double SingleVoxelSizeHalfDouble  { 50.0   };
static constexpr int32  SingleVoxelSizeHalfInteger { 50     };

static constexpr float  JToUScale        { 100.0f                               };
static constexpr float  UToJScale        { 1.0f / WorldStatics::JToUScale       };
static constexpr double JToUScaleDouble  { 100.0                                };
static constexpr double UToJScaleDouble  { 1.0  / WorldStatics::JToUScaleDouble };
static constexpr int    JToUScaleInteger { 100                                  };
/* There is obviously no integer with a U To J Scale. */

}
