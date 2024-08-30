// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/**
 * File that defines and re-defines macros useful for testing.
 * Never include this file directly - include JAFGMacros.h instead.
 */
#ifndef INCLUDED_JAFG_MACROS_HEADER
    #error "This file should never be included directly. Include JAFGMacros.h instead."
#endif /* !INCLUDED_JAFG_MACROS_HEADER */

/** Do not use. Only for other private macros. */
#define CHECK_FAILED_THROW_MSG_PRIVATE "__CHECK_FAILED"

#ifdef check
    #if DO_CHECK_ASSERTIONS

        #undef check

        /**
         * Redefine check assertions to throw an error. This is maybe useful for some unit tests.
         * This is a little bit of a hack, as it affects literally all checks in the application and
         * should be avoided if possible.
         */
        // ReSharper disable once CppUE4CodingStandardNamingViolationWarning
        #define check(expr)                                           \
        {                                                             \
            if (UNLIKELY(!(expr)))                                    \
            {                                                         \
                throw std::exception(CHECK_FAILED_THROW_MSG_PRIVATE); \
            }                                                         \
        }

        /**
         * Very basic method to check if an assertion is thrown.
         */
        #define CHECK_ASSERT(What, X)                                                                           \
        {                                                                                                       \
            bool bThrown##__LINE__ = false;                                                                     \
            try                                                                                                 \
            {                                                                                                   \
                X;                                                                                              \
            }                                                                                                   \
            catch (const std::exception& e)                                                                     \
            {                                                                                                   \
                bThrown##__LINE__ = true;                                                                       \
                LOG_VERY_VERBOSE(LogUnitTests, "Caught exception [%hs]. On: [%s].", e.what(), What)             \
            }                                                                                                   \
            if (bThrown##__LINE__ == false)                                                                     \
            {                                                                                                   \
                CHECK_EQUALS( FString::Printf(TEXT("Expected assertion failure for [%s]"), What), false, true ) \
            }                                                                                                   \
        }
    #else /* DO_CHECK_ASSERTIONS */
        #define CHECK_ASSERT(What, X)
    #endif /* !DO_CHECK_ASSERTIONS */
#else /* check */
    /** Just obliterate the marco for now. Maybe we can do a fallback check later? */
    #define CHECK_ASSERT(What, X)
#endif /* !check */
