// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/**
 * Use this macro in a header file to make the IDEA shut up about false positive
 * possibly unused #include directive warnings.
 */
#ifndef JAFG_VOID
    #define JAFG_VOID
#endif /* JAFG_VOID */


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

#include "Runtime/JAFGBuild.h"
#include "JAFGMacros.h"
#include "JAFGTypeDefs.h"
#include "JAFGTypes.h"
#include "VoxelMask.h"
#include "Accumulated.h"
#include "RegisteredWorldNames.h"
#include "System/RecipeSubsystem.h"


/*----------------------------------------------------------------------------
    Commonly used extern plugins headers.
----------------------------------------------------------------------------*/

#include "JAFGLogDefs.h"
