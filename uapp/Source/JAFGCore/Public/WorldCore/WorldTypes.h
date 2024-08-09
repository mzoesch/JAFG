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

FORCEINLINE auto LexToString(const EChunkType::Type ChunkType) -> FString
{
    switch (ChunkType)
    {
    case EChunkType::Greedy:
    {
        return TEXT("Greedy");
    }
    default:
    {
        jrelaxedCheckNoEntry()
        return TEXT("Unknown");
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

FORCEINLINE auto LexToString(const EWorldGenerationType::Type GenerationType) -> FString
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
        jrelaxedCheckNoEntry()
        return TEXT("Unknown");
    }
    }
}
