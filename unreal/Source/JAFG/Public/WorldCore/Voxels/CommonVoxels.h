// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/* Part of the MyCore package. */
#include "CoreMinimal.h"
#include "System/TypeDefs.h"

namespace ECommonVoxels
{

enum Type : uint32
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
#endif /* !UE_BUILD_SHIPPING */

}
