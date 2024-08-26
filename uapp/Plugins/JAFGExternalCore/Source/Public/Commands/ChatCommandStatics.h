// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGExternalCoreIncludes.h"

JAFG_VOID

/**
 * Commands are always case-insensitive.
 * Prefixes are not mandatory. Except for the command prefix.
 */
#if 0

/* Both will be treated as the same command. */

inline FString ExampleCommandWithPrefix    = TEXT("sv_ExampleCommand");
inline FString ExampleCommandWithoutPrefix = TEXT("ExampleCommand");

#endif /* 0 */

typedef uint8   CommandReturnCode;
typedef FString FChatCommand;

namespace ECommandReturnCode { enum Type : CommandReturnCode; }

FString JAFGEXTERNALCORE_API LexToString(const ECommandReturnCode::Type& InType);

namespace ECommandReturnCode
{

enum Type : CommandReturnCode
{
    /** Marks an invalid state of a return code. */
    Invalid           = 0,

    //////////////////////////////////////////////////////////////////////////
    // Somehow Successful Entries
    //////////////////////////////////////////////////////////////////////////

    /** Command executed successfully. The parser might want to decide to give user feedback with the command's response. */
    Success,

    /** Command executed successfully. The parser is advised to not give method feedback to the user. */
    SuccessNoResponse,

    /** Command executed successfully. The parser is advised to broadcast the command's response. */
    SuccessBroadcast,

    /** Command executed successfully. The parser is advised to broadcast the command's response with the authority as sender. */
    SuccessBroadcastWithAuthority,

    //////////////////////////////////////////////////////////////////////////
    // Failure Entries - In rising order of severity
    //////////////////////////////////////////////////////////////////////////

    /*
     * It is always safe to just compare with greater or equal to ECommandReturnCodes::Failure to be sure to catch all
     * failure states.
     */

    /** Command executed with an unknown failure. */
    Failure,

    /** The command was not found in the subsystem's registry. */
    Unknown,

    /** Command cannot be executed due to user rights, or the current game state is blocking the command. */
    Forbidden,

    /** Command executed with missing arguments. */
    MissingArgs,

    /** Command executed with too many arguments. */
    TooManyArgs,

    /** Command executed with a semantic error. */
    SemanticError,

    /** Command executed with a syntax error. */
    SyntaxError,

    /** Platform reports errors or blocks request. */
    PlatformError,
};

FORCEINLINE JAFGEXTERNALCORE_API bool IsSuccess(const ECommandReturnCode::Type& InType)
{
    return
           InType == ECommandReturnCode::Success
        || InType == ECommandReturnCode::SuccessNoResponse
        || InType == ECommandReturnCode::SuccessBroadcast
        || InType == ECommandReturnCode::SuccessBroadcastWithAuthority;
}

FORCEINLINE JAFGEXTERNALCORE_API bool IsSuccess(const CommandReturnCode& InType)
{
    return ECommandReturnCode::IsSuccess(static_cast<ECommandReturnCode::Type>(InType));
}

FORCEINLINE JAFGEXTERNALCORE_API bool IsFailure(const ECommandReturnCode::Type& InType)
{
    return InType >= ECommandReturnCode::Failure;
}

FORCEINLINE JAFGEXTERNALCORE_API bool IsFailure(const CommandReturnCode& InType)
{
    return ECommandReturnCode::IsFailure(static_cast<ECommandReturnCode::Type>(InType));
}

}

FORCEINLINE JAFGEXTERNALCORE_API auto LexToString(const CommandReturnCode& InType) -> FString { return LexToString(static_cast<ECommandReturnCode::Type>(InType)); }
FORCEINLINE JAFGEXTERNALCORE_API auto LexToString(const ECommandReturnCode::Type& InType) -> FString
{
    switch (InType)
    {
    case ECommandReturnCode::Invalid:
    {
        return TEXT("Invalid");
    }
    case ECommandReturnCode::Success:
    {
        return TEXT("Success");
    }
    case ECommandReturnCode::SuccessNoResponse:
    {
        return TEXT("SuccessNoResponse");
    }
    case ECommandReturnCode::SuccessBroadcast:
    {
        return TEXT("SuccessBroadcast");
    }
    case ECommandReturnCode::SuccessBroadcastWithAuthority:
    {
        return TEXT("SuccessBroadcastWithAuthority");
    }
    case ECommandReturnCode::Failure:
    {
        return TEXT("Failure");
    }
    case ECommandReturnCode::Unknown:
    {
        return TEXT("Unknown");
    }
    case ECommandReturnCode::Forbidden:
    {
        return TEXT("Forbidden");
    }
    case ECommandReturnCode::MissingArgs:
    {
        return TEXT("MissingArgs");
    }
    case ECommandReturnCode::TooManyArgs:
    {
        return TEXT("TooManyArgs");
    }
    case ECommandReturnCode::SemanticError:
    {
        return TEXT("SemanticError");
    }
    case ECommandReturnCode::SyntaxError:
    {
        return TEXT("SyntaxError");
    }
    case ECommandReturnCode::PlatformError:
    {
        return TEXT("PlatformError");
    }
    default:
    {
        checkNoEntry()
        return TEXT("Unknown");
    }
    }
}

namespace CommandStatics
{

static const FString CommandPrefix       = TEXT("/");
/* Has to be the same lenght as client prefix. */
static const FString ServerCommandPrefix = TEXT("sv_");
/* Has to be the same lenght as server prefix. */
static const FString ClientCommandPrefix = TEXT("cl_");

static const int32 PlatformCommandPrefixLength = ServerCommandPrefix.Len();

JAFGEXTERNALCORE_API bool IsCommand(const FText& StdIn);
JAFGEXTERNALCORE_API bool IsServerCommand(const FText& StdIn);
JAFGEXTERNALCORE_API bool IsClientCommand(const FText& StdIn);

JAFGEXTERNALCORE_API FString GetCommand(const FText& StdIn);
JAFGEXTERNALCORE_API FString GetCommandWithServerPrefix(const FText& StdIn);
JAFGEXTERNALCORE_API FString SafePrefixServerCommand(const FString& Command);
JAFGEXTERNALCORE_API FString GetCommandWithClientPrefix(const FText& StdIn);
JAFGEXTERNALCORE_API FString SafePrefixClientCommand(const FString& Command);
JAFGEXTERNALCORE_API FString GetCommandWithArgs(const FText& StdIn, TArray<FString>& OutArgs);

JAFGEXTERNALCORE_API bool DoesCommandStartWithSpecificCommandType(const FChatCommand& Command, bool& bOutClientPrefix);
JAFGEXTERNALCORE_API bool DoesStdInStartWithSpecificCommandType(const FText& StdIn, bool& bOutClientPrefix);

}
