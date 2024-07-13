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
#define DO_RELAXED_JCHECKS 1

#define WITH_STRIKE_SUBSYSTEM 0
