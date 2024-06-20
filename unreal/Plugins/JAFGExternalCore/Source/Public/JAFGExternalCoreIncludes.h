// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/*
 * Add this file to get access to the most common headers and types.
 */


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

#include "JAFGMacros.h"
#include "JAFGTypeDefs.h"
#include "JAFGTypes.h"
#include "VoxelMask.h"
#include "Accumulated.h"
#include "RegisteredWorldNames.h"


/*----------------------------------------------------------------------------
    Commonly used extern plugins headers.
----------------------------------------------------------------------------*/

#include "JAFGLogDefs.h"
