// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGLogDefs.h"

/**
 * A check that is always executed even in shipping builds.
 */
#if DO_CHECK
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheck(expr) check(expr)
#else /* DO_CHECK */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheck(expr)                                            \
    {                                                               \
        if ((!!(!(expr))))                                          \
        {                                                           \
            LOG_FATAL(LogSystem, "jcheck failed with [%s].", #expr) \
        }                                                           \
    }
#endif /* !DO_CHECK */

/**
 * A no entry check that is always executed even in shipping builds.
 */
#if DO_CHECK
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheckNoEntry() checkNoEntry()
#else /* DO_CHECK */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheckNoEntry()                                             \
    {                                                                   \
        LOG_FATAL(LogSystem, "Enclosing block should never be called.") \
    }
#endif /* !DO_CHECK */

/* TODO: Make this throwable. */
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
#define jrelaxedCheckNoEntry()                                      \
    LOG_ERROR(LogSystem, "Enclosing block should never be called.")
