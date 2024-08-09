// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGMacros.h"
#include "JAFGTypeDefs.h"

/**
 * Note, that this is the very same as ECommonVoxels. It is just a different namespace for some cleaner implementation
 * and better readability. But JAFG does not give a fuck about the type of Accumulated behind the scenes.
 * Meaning it does not differentiate between Voxels and Items.
 */
namespace ECommonAccumulated
{

enum Type : voxel_t
{
    Null = 0,
    Max  = 0,
    Num  = 1,
};

}

namespace ECommonVoxels
{

enum Type : voxel_t
{
    Null = 0,
    Air  = 1,
    Max  = 1,
    Num  = 2,
};

#if !UE_BUILD_SHIPPING
/**
 * It is the foundation to fast testing and development and only a helper
 * method to a temporary fix.
 */
FORCEINLINE voxel_t GetBaseVoxel(void)
{
    return ECommonVoxels::Max + 1;
}
#else /* !UE_BUILD_SHIPPING */
/**
 * Kinda interesting here...
 */
FORCEINLINE voxel_t GetBaseVoxel(void)
{
    return ECommonVoxels::Air;
}
#endif /* UE_BUILD_SHIPPING */

}

namespace ETextureGroup
{

enum Type : int8
{
    Core   = -1,
    Opaque =  0,
    /* All other groups are determined at runtime. See the MaterialSubsystem.h for more information. */
};

/**
 * @param BlendArrIdx The index in the blend array (see MaterialSubsystem.h for more information).
 * @return The texture group.
 */
FORCEINLINE ETextureGroup::Type FromBlendArrIdx(const int32 BlendArrIdx)
{
    /*
     * Currently this is really simple, but as we will increase groups (like transparent, animated, etc.) this
     * will get a bit more complex.
     */
    return static_cast<ETextureGroup::Type>(BlendArrIdx + 1);
}

}

namespace ENormalLookup
{

enum Type : int8
{
    Default,
    Top,
    Bottom,
    Front,
    Side,
};

FORCEINLINE ENormalLookup::Type FromVector(const FVector& Normal)
{
    if (Normal == FVector::UpVector)
    {
        return ENormalLookup::Top;
    }

    if (Normal == FVector::DownVector)
    {
        return ENormalLookup::Bottom;
    }

    if (Normal == FVector::ForwardVector || Normal == FVector::BackwardVector)
    {
        return ENormalLookup::Front;
    }

    if (Normal == FVector::RightVector || Normal == FVector::LeftVector)
    {
        return ENormalLookup::Side;
    }

    jcheck( false && "The normal vector is not valid." )
    return ENormalLookup::Default;
}

FORCEINLINE bool IsValid(const FString& Normal)
{
    return Normal == TEXT("Top") || Normal == TEXT("Bot") || Normal == TEXT("Front") || Normal == TEXT("Side");
}

FORCEINLINE ENormalLookup::Type FromString(const FString& Normal)
{
    if (Normal == TEXT("Top"))
    {
        return ENormalLookup::Top;
    }

    if (Normal == TEXT("Bot"))
    {
        return ENormalLookup::Bottom;
    }

    if (Normal == TEXT("Front"))
    {
        return ENormalLookup::Front;
    }

    if (Normal == TEXT("Side"))
    {
        return ENormalLookup::Side;
    }

    jcheck( false && "The normal string is not valid." )
    return ENormalLookup::Default;
}

}

static const FString CommonNamespace = TEXT("COMMON");

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
