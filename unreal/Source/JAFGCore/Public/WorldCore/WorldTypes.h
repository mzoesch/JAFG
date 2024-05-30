// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"
#include "JAFGMacros.h"

#include "WorldTypes.generated.h"

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
        jcheck( false && "Unknown generation type." )
        return TEXT("Unknown");
    }
    }
}

}
