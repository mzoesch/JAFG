// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/**
 * If true, the application will always crash when a jcheck or jcheck-like macro fails.
 * If false, only the error-reporter will be notified.
 *
 * @note The setting is ignored if DO_CHECK is true.
 */
#define DO_HARSH_JCHECK 1

/**
 * If true, the application will not crash but relax when a jrelaxedCheck or jrelaxedCheck-like macro fails.
 * Only the error-reporter will be notified.
 * If false, the application will treat the relaxed checks as normal jchecks.
 */
#if WITH_EDITOR
    #define DO_RELAXED_JCHECKS 1
#else /* WITH_EDITOR */
    #define DO_RELAXED_JCHECKS 0
#endif /* !WITH_EDITOR */

/**
 * If true, the application will throw an error when a very relaxed jcheck or jcheck-like macro fails.
 * Only the error-reporter will be notified.
 * If false, the compiler will purge the very relaxed checks from the code.
 */
#if UE_BUILD_SHIPPING
    #define DO_VERY_RELAXED_JCHECKS 0
#else
    #define DO_VERY_RELAXED_JCHECKS 1
#endif /* !UE_BUILD_SHIPPING */

#define WITH_STRIKE_SUBSYSTEM 0

/** When running unit tests, do we want to run checks that check if specific assertions are being triggered? */
/*
 * We check here for not defined - as the default value is false.
 * Some unit tests might want to override this value only for them and not the whole application.
 */
#ifndef DO_CHECK_ASSERTIONS
    #if WITH_TESTS && !UE_BUILD_SHIPPING
        #define DO_CHECK_ASSERTIONS 0
    #else /* WITH_TESTS && !UE_BUILD_SHIPPING */
        #define DO_CHECK_ASSERTIONS 0
    #endif /* !(WITH_TESTS && !UE_BUILD_SHIPPING) */
#endif /* !DO_CHECK_ASSERTIONS */
