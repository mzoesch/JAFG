// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "WorldTypes.generated.h"

extern inline JAFG_API const FString CommonNamespace = TEXT("COMMON");
/**
 * Kinda sketchy because JAFG is just a "mod" onto this game. That should always be (in theory) replaceable by
 * any other third-party mod.
 * This is so that we can drastically split the game's code and enforce code separation. Makes it overall more
 * maintainable and easier to understand, debug and write.
 * So whenever referencing JAFG, this should be done only in the JAFG namespace.
 * But never in the game's namespace.
 */
extern inline JAFG_API const FString JAFGNamespace   = TEXT("JAFG");

UENUM(BlueprintType)
namespace EChunkType
{

enum Type : uint8
{
    Greedy,
};

}

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
