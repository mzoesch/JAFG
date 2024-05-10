// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/**
 * Use this macro in a header file to make the IDEA shut up about false positive
 * possibly unused #include directive warnings.
 */
#define JAFG_VOID


/*----------------------------------------------------------------------------
    Engine includes.
----------------------------------------------------------------------------*/

#include "CoreMinimal.h"


/*----------------------------------------------------------------------------
    Commonly used headers.
----------------------------------------------------------------------------*/

/*
 * This is all super, super like really super cheeky and dirty.
 * All these includes are sometimes using each other.
 * That's why we define the most import stuff really early and then the rest.
 *
 * So change the order with caution.
 */

#include "System/TypeMacros.h"
#include "System/Types.h"
#include "Jar/Accumulated.h"
#include "WorldCore/Voxels/CommonVoxels.h"
#include "WorldCore/WorldTypes.h"
#include "WorldCore/Chunk/ChunkStatics.h"


/*----------------------------------------------------------------------------
    Commonly used extern modules headers.
----------------------------------------------------------------------------*/

#include "JAFGLogging/Public/Definitions.h"
#include "JAFGNetCore/Public/CommonNetworkStatics.h"
