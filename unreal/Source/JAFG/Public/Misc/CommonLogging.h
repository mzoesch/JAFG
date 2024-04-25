// Copyright 2024 mzoesch. All rights reserved.

#pragma once

/**
 * Quick switch.
 * If turned on, all logs that use this macro library will output the line number of the log in addition to the
 * class, if existing, and function name.
 */
#define LOG_WITH_LINE_NUMBERS 0

#define UNDEF_PRIVATE_LOGS    0

/*
 * Note to self;
 *
 * Enable verbose logging in the Default Engine INI file.
 */


/*----------------------------------------------------------------------------
    Common extern logging categories.
----------------------------------------------------------------------------*/

DECLARE_LOG_CATEGORY_EXTERN(LogChunkGeneration, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogChunkManipulation, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogChunkMisc, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogChunkValidation, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogCommonSlate, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogEntitySystem, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogGenPrevAssets, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogWorldGameMode, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogHyperlane, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogVoxelSubsystem, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogWorldChar, Log, All)


/*----------------------------------------------------------------------------
    Internal and private logging macros.
----------------------------------------------------------------------------*/

/**
 * Current class name.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS \
    (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))))

/**
 * Current function name.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_FUNC \
    (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2))

/**
 * Current function signature.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_FUNC_SIG \
    (FString(__FUNCSIG__))

/**
 * Current line number.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_LINE \
    (FString::FromInt(__LINE__))

/**
 * Current class and function name.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC \
    (FString(__FUNCTION__))

/**
 * Current class name, function name and line number.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE \
    (LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC + L"[" + LOG_PRIVATE_TRACE_STR_CUR_LINE + L"]")


/*----------------------------------------------------------------------------
    External logging macros.
----------------------------------------------------------------------------*/

#if LOG_WITH_LINE_NUMBERS

/**
 * A macro that logs a formatted message if the log category is active at VERY VERBOSE verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_VERY_VERBOSE(CategoryName, Format, ...) \
    UE_LOG(CategoryName, VeryVerbose, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at VERBOSE verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_VERBOSE(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Verbose, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at DISPLAY verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_DISPLAY(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Display, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at WARNING verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_WARNING(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Warning, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at ERROR verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_ERROR(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Error, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at FATAL verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_FATAL(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Fatal, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

#else /* LOG_WITH_LINE_NUMBERS */

/**
 * A macro that logs a formatted message if the log category is active at VERY VERBOSE verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_VERY_VERBOSE(CategoryName, Format, ...) \
    UE_LOG(CategoryName, VeryVerbose, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at VERBOSE verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_VERBOSE(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Verbose, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at DISPLAY verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_DISPLAY(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Display, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at WARNING verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_WARNING(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Warning, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at ERROR verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_ERROR(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Error, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

/**
 * A macro that logs a formatted message if the log category is active at FATAL verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DEFINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of printf.
 */
#define LOG_FATAL(CategoryName, Format, ...) \
    UE_LOG(CategoryName, Fatal, TEXT("%s: %s"), *LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, *FString::Printf(TEXT(Format), ##__VA_ARGS__ ) )

#endif /* !LOG_WITH_LINE_NUMBERS */

#undef LOG_WITH_LINE_NUMBERS

#if UNDEF_PRIVATE_LOGS

#undef LOG_PRIVATE_TRACE_STR_CUR_CLASS
#undef LOG_PRIVATE_TRACE_STR_CUR_FUNC
#undef LOG_PRIVATE_TRACE_STR_CUR_FUNC_SIG
#undef LOG_PRIVATE_TRACE_STR_CUR_LINE
#undef LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC
#undef LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE

#endif /* UNDEF_PRIVATE_LOGS */

#undef UNDEF_PRIVATE_LOGS
