// Copyright 2024 mzoesch. All rights reserved.

#pragma once

namespace ECommonVoxels
{

enum Type : uint32
{
    Null = 0,
    Air  = 1,
};

/*
 * Never use this method in production code. It is the foundation to fast testing and development and only a helper
 * method to a temporary fix.
 */
FORCEINLINE uint32 GetBaseVoxel(void)
{
// #if !WITH_EDITOR /* && !UE_BUILD_SHIPPING */
//     LOG_FATAL(LogChunkMisc, "Disallowed.")
// #endif /* !WITH_EDITOR */
    return ECommonVoxels::Air + 1;
}

}
