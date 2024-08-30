// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGLogDefs.h"
#include "Runtime/JAFGBuild.h"

/**
 * Some files should never be included directly but only conditionally by including this file.
 * They then can check if they are included at a later point than this master macro repository file.
 */
#define INCLUDED_JAFG_MACROS_HEADER

//////////////////////////////////////////////////////////////////////////
// Static assertions

#ifndef DO_HARSH_JCHECK
    #error "DO_HARSH_JCHECK is not defined."
#endif /* !DO_HARSH_JCHECK */

#ifndef DO_RELAXED_JCHECKS
    #error "DO_RELAXED_JCHECKS is not defined."
#endif /* !DO_RELAXED_JCHECKS */

#ifndef DO_VERY_RELAXED_JCHECKS
    #error "DO_VERY_RELAXED_JCHECKS is not defined."
#endif /* !DO_VERY_RELAXED_JCHECKS */

// ~Static assertions
//////////////////////////////////////////////////////////////////////////

#define UNREACHABLE_ENCLOSING_BLOCK_TEXT "Enclosing block should never be called."

/**
 * A check that is always executed even in shipping builds.
 */
#if DO_CHECK
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheck(expr) check(expr)
#else /* DO_CHECK */
    #if DO_HARSH_JCHECK
        // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
        #define jcheck(expr)                                            \
        {                                                               \
            if (UNLIKELY(!(expr)))                                      \
            {                                                           \
                LOG_FATAL(LogSystem, "jcheck failed with [%s].", #expr) \
            }                                                           \
        }
    #else /* DO_HARSH_JCHECK */
        // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
        #define jcheck(expr)                                            \
        {                                                               \
            if (UNLIKELY(!(expr)))                                      \
            {                                                           \
                LOG_ERROR(LogSystem, "jcheck failed with [%s].", #expr) \
            }                                                           \
        }
    #endif /* !DO_HARSH_JCHECK */
#endif /* !DO_CHECK */

/**
 * A check that only throws an error. If DO_RELAXED_JCHECKS is false, it will be treated
 * by the compiler as a normal jcheck.
 */
#if DO_RELAXED_JCHECKS
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jrelaxedCheck(expr)                                            \
    {                                                                      \
        if (UNLIKELY(!(expr)))                                             \
        {                                                                  \
            LOG_ERROR(LogSystem, "jrelaxedCheck failed with [%s].", #expr) \
        }                                                                  \
    }
#else /* DO_RELAXED_JCHECKS */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jrelaxedCheck(expr) jcheck(expr)
#endif /* !DO_RELAXED_JCHECKS */

/**
 * A check that only throws an error. If DO_VERY_RELAXED_JCHECKS is false,
 * it will be completely purged from the code.
 */
#if DO_VERY_RELAXED_JCHECKS
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jveryRelaxedCheck(expr)                                            \
    {                                                                          \
        if (UNLIKELY(!(expr)))                                                 \
        {                                                                      \
            LOG_ERROR(LogSystem, "jveryRelaxedCheck failed with [%s].", #expr) \
        }                                                                      \
    }
#else /* DO_VERY_RELAXED_JCHECKS */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jveryRelaxedCheck(expr)
#endif /* !DO_VERY_RELAXED_JCHECKS */

/**
 * A no entry check that is always executed even in shipping builds.
 */
#if DO_CHECK
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jcheckNoEntry() checkNoEntry()
#else /* DO_CHECK */
    #if DO_HARSH_JCHECK
        // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
        #define jcheckNoEntry()                                    \
        {                                                          \
            LOG_FATAL(LogSystem, UNREACHABLE_ENCLOSING_BLOCK_TEXT) \
        }
    #else /* DO_HARSH_JCHECK */
        // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
        #define jcheckNoEntry()                                    \
        {                                                          \
            LOG_ERROR(LogSystem, UNREACHABLE_ENCLOSING_BLOCK_TEXT) \
        }
    #endif /* !DO_HARSH_JCHECK */
#endif /* !DO_CHECK */

/**
 * A no entry check that only throws an error. If DO_RELAXED_JCHECKS is false, it
 * will be treated by the compiler as a normal jcheck.
 */
#if DO_RELAXED_JCHECKS
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jrelaxedCheckNoEntry() \
        LOG_ERROR(LogSystem, UNREACHABLE_ENCLOSING_BLOCK_TEXT)
#else /* DO_RELAXED_JCHECKS */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jrelaxedCheckNoEntry() \
        jcheckNoEntry()
#endif /* !DO_RELAXED_JCHECKS */

/**
 * A no entry check that only throws an error. If DO_VERY_RELAXED_JCHECKS is false,
 * it will be completely purged from the code.
 */
#if DO_VERY_RELAXED_JCHECKS
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jveryRelaxedCheckNoEntry() \
        LOG_ERROR(LogSystem, UNREACHABLE_ENCLOSING_BLOCK_TEXT)
#else /* DO_VERY_RELAXED_JCHECKS */
    // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
    #define jveryRelaxedCheckNoEntry()
#endif /* !DO_VERY_RELAXED_JCHECKS */

/**
 * Only include this file when testing to override defined macros.
 */
#if WITH_TESTS
    #if UE_BUILD_SHIPPING
        #error "Found WITH_TESTS but not in a test build."
    #endif /* UE_BUILD_SHIPPING */
    #include "JAFGTestMacros.h"
#endif /* WITH_TESTS */
