// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/*
 * Commands are always case-insensitive.
 * Prefixes are not mandatory. Except for the command prefix.
 */

typedef uint8 CommandReturnCode;

namespace ECommandReturnCodes
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

    /** Command executed with a syntax error. */
    SyntaxError,

    /** Platform reports errors or blocks request. */
    PlatformError,
};

FORCEINLINE bool IsSuccess(const ECommandReturnCodes::Type& InType)
{
    return
           InType == ECommandReturnCodes::Success
        || InType == ECommandReturnCodes::SuccessNoResponse
        || InType == ECommandReturnCodes::SuccessBroadcast
        || InType == ECommandReturnCodes::SuccessBroadcastWithAuthority;
}

FORCEINLINE bool IsSuccess(const CommandReturnCode& InType)
{
    return ECommandReturnCodes::IsSuccess(static_cast<ECommandReturnCodes::Type>(InType));
}

FORCEINLINE bool IsFailure(const ECommandReturnCodes::Type& InType)
{
    return InType >= ECommandReturnCodes::Failure;
}

FORCEINLINE bool IsFailure(const CommandReturnCode& InType)
{
    return ECommandReturnCodes::IsFailure(static_cast<ECommandReturnCodes::Type>(InType));
}

FORCEINLINE FString LexToString(const ECommandReturnCodes::Type& InType)
{
    switch (InType)
    {
    case ECommandReturnCodes::Invalid:
    {
        return TEXT("Invalid");
    }
    case ECommandReturnCodes::Success:
    {
        return TEXT("Success");
    }
    case ECommandReturnCodes::SuccessNoResponse:
    {
        return TEXT("SuccessNoResponse");
    }
    case ECommandReturnCodes::SuccessBroadcast:
    {
        return TEXT("SuccessBroadcast");
    }
    case ECommandReturnCodes::SuccessBroadcastWithAuthority:
    {
        return TEXT("SuccessBroadcastWithAuthority");
    }
    case ECommandReturnCodes::Failure:
    {
        return TEXT("Failure");
    }
    case ECommandReturnCodes::Unknown:
    {
        return TEXT("Unknown");
    }
    case ECommandReturnCodes::Forbidden:
    {
        return TEXT("Forbidden");
    }
    case ECommandReturnCodes::MissingArgs:
    {
        return TEXT("MissingArgs");
    }
    case ECommandReturnCodes::TooManyArgs:
    {
        return TEXT("TooManyArgs");
    }
    case ECommandReturnCodes::SyntaxError:
    {
        return TEXT("SyntaxError");
    }
    case ECommandReturnCodes::PlatformError:
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

FORCEINLINE FString LexToString(const CommandReturnCode& InType)
{
    return ECommandReturnCodes::LexToString(static_cast<ECommandReturnCodes::Type>(InType));
}

}

namespace CommandStatics
{


static const FString CommandPrefix       = TEXT("/");
static const FString ServerCommandPrefix = TEXT("sv_");
static const FString ClientCommandPrefix = TEXT("cl_");

bool IsCommand(const FText& StdIn);
bool IsServerCommand(const FText& StdIn);
bool IsClientCommand(const FText& StdIn);

FString GetCommand(const FText& StdIn);
FString GetCommandWithServerPrefix(const FText& StdIn);
FString SafePrefixServerCommand(const FString& Command);
FString GetCommandWithClientPrefix(const FText& StdIn);
FString SafePrefixClientCommand(const FString& Command);
FString GetCommandWithArgs(const FText& StdIn, TArray<FString>& OutArgs);

}

namespace ChatStatics
{

static const FString AuthorityName       = TEXT("AUTHORITY");
static const FString InternalName        = TEXT("INTERNAL");

static constexpr int32 MaxChatInputLength = 0x7F;

FORCEINLINE bool IsTextToLong(const FText& Text)
{
    return Text.ToString().Len() > MaxChatInputLength;
}

FORCEINLINE bool IsTextBlank(const FText& Text)
{
    for (const TCHAR& Char : Text.ToString())
    {
        if (Char != TEXT(' '))
        {
            return false;
        }
    }

    return true;
}

FORCEINLINE bool IsTextValid(const FText& Text)
{
    return
           Text.ToString().Len()           >= 1
        && Text.ToString().IsEmpty()       == false
        && ChatStatics::IsTextBlank(Text)  == false
        && ChatStatics::IsTextToLong(Text) == false;
}

}