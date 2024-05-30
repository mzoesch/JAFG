// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGLogDefs.h"

/**
 * A check that is always executed even in shipping builds.
 */
#if DO_CHECK
    #define jcheck(expr) check(expr)
#else
    #define jcheck(expr)                                            \
    {                                                               \
        if ((!!(!(expr))))                                          \
        {                                                           \
            LOG_FATAL(LogSystem, "jcheck failed with [%s].", #expr) \
        }                                                           \
    }
#endif
