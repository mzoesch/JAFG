// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"

#include "WorldTypes.generated.h"

extern inline JAFG_API const FString CommonNamespace = TEXT("COMMON");

UENUM(BlueprintType)
namespace EChunkType
{

enum Type : uint8
{
    Greedy,
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
extern inline JAFG_API constexpr int32 ChunkSize = 16;

extern inline JAFG_API constexpr float  JToUScale        { 100.0f                               };
extern inline JAFG_API constexpr float  UToJScale        { 1.0f / WorldStatics::JToUScale       };
extern inline JAFG_API constexpr double JToUScaleDouble  { 100.0                                };
extern inline JAFG_API constexpr double UToJScaleDouble  { 1.0  / WorldStatics::JToUScaleDouble };
extern inline JAFG_API constexpr int    JToUScaleInteger { 100                                  };
/* There is obviously no integer with a U To J Scale. */

}
