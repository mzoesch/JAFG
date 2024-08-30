// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "JAFGExternalCoreIncludes.h"
#include "Commands/ChatCommandStatics.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ShippedWorldChatCommands.generated.h"

JAFG_VOID

#define DEFAULT_SYNTAX_RET_LIMIT                                                \
    20
#define DECLARE_CLIENT_COMMAND(Name, Callback, Subsystem)                       \
    {                                                                           \
        const FChatCommand Command = FString::Printf(                           \
            TEXT("%s%s"),                                                       \
            *CommandStatics::ClientCommandPrefix, TEXT(Name)                    \
        );                                                                      \
        Subsystem->RegisterCommand(Command, [] (FCommandParams& Params) -> void \
        {                                                                       \
            Callback(Params);                                                   \
        });                                                                     \
    }

class  UChatComponent;
struct FChatCommandParams;

typedef TFunction<void(FChatCommandParams& Params)>                       FChatCommandCallback;
typedef FString                                                           FSyntaxIdentifier;
typedef TFunction<TArray<FString>(const FSyntaxIdentifier& InPieceIdent)> FSyntaxCallback;
typedef int32                                                             FArgCursor;

struct JAFGEXTERNALCORE_API FChatCommandParams final
{
    FChatCommandParams(UChatComponent* InCallee, TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        : Callee(InCallee), InArgs(InArgs), OutReturnCode(OutReturnCode), OutResponse(OutResponse)
    {
    }

    UChatComponent*    Callee;
    TArray<FString>&   InArgs;
    CommandReturnCode& OutReturnCode;
    FString&           OutResponse;
};

namespace EChatCommandSyntax
{

enum Type : uint16
{
    //////////////////////////////////////////////////////////////////////////
    // Typedefs
    //////////////////////////////////////////////////////////////////////////

    Any             = 0,
    SharpSharpAny,

    PlayerName,

    Location,

    Accumulated,
    Voxel,
    Item,

    Byte,
    Integer32,
    UInteger32,

    //////////////////////////////////////////////////////////////////////////
    // Redirected typedefs
    //////////////////////////////////////////////////////////////////////////

    /** Redirects to Integer32  */
    Integer         = 100,
    /** Redirects to UInteger32 */
    AccAmount,

    //////////////////////////////////////////////////////////////////////////
    // Subsystem redirections
    //////////////////////////////////////////////////////////////////////////

    Custom          = 200,
};

JAFGEXTERNALCORE_API auto Redirect(const EChatCommandSyntax::Type& InType) -> EChatCommandSyntax::Type;

FORCEINLINE JAFGEXTERNALCORE_API bool IsTypedef(const Type& InType)
{
    return InType < EChatCommandSyntax::Integer;
}

FORCEINLINE JAFGEXTERNALCORE_API bool IsRedirected(const Type& InType)
{
    return InType >= EChatCommandSyntax::Integer && InType < EChatCommandSyntax::Custom;
}

FORCEINLINE JAFGEXTERNALCORE_API bool IsSubsystemRedirected(const Type& InType)
{
    return InType >= EChatCommandSyntax::Custom;
}

}

JAFGEXTERNALCORE_API FString LexToString(const EChatCommandSyntax::Type& InType);

struct JAFGEXTERNALCORE_API FChatCommandObject final
{
    FChatCommandObject(void) = default;

    FChatCommand                     Command;
    FChatCommandCallback             Callback;
    FString                          Info;
    TArray<EChatCommandSyntax::Type> Syntax;
};

namespace CommandStatics::Syntax
{

/**
 * @return True, if the currently pointed to argument was parsed correctly (the syntax was ok).
 *         This does not mean that the output is valid and is semantically correct.
 */
JAFGEXTERNALCORE_API bool ParseArgument(
    const UWorld& InContext,
    const TArray<FString>& InArgs,
    const EChatCommandSyntax::Type& InArgSyntax,
    FString& OutArgStringRepresentation,
    FArgCursor& MovableArgCursor,
    bool& bBadInput
);

JAFGEXTERNALCORE_API void GetAllAvailableInputsForSyntax(
    const UWorld& InContext,
    const EChatCommandSyntax::Type& InSyntax,
    const TArray<FString>& InArgs,
    const FArgCursor InArgCursor,
    TArray<FString>& OutPossibleInputs,
    const int32 InLimit = DEFAULT_SYNTAX_RET_LIMIT
);

//////////////////////////////////////////////////////////////////////////
// Specific syntaxes
//////////////////////////////////////////////////////////////////////////

JAFGEXTERNALCORE_API void GetAllAvailableInputsForSyntax_PlayerName(
    const UWorld& InContext,
    TArray<FString>& OutPossibleInputs,
    const TArray<FString>* InArgs = nullptr,
    const FArgCursor* InArgCursor = nullptr,
    const int32 InLimit = DEFAULT_SYNTAX_RET_LIMIT
);

JAFGEXTERNALCORE_API void GetAllAvailableInputsForSyntax_Accumulated(
    const UWorld& InContext,
    TArray<FString>& OutPossibleInputs,
    const TArray<FString>* InArgs = nullptr,
    const FArgCursor* InArgCursor = nullptr,
    const int32 InLimit = DEFAULT_SYNTAX_RET_LIMIT
);

//////////////////////////////////////////////////////////////////////////
// Common syntaxes determination methods
//////////////////////////////////////////////////////////////////////////

JAFGEXTERNALCORE_API auto IsNumeric(const FString& S) -> bool;

JAFGEXTERNALCORE_API auto IsByte(const FString& S) -> bool;
JAFGEXTERNALCORE_API auto ToByte(const FString& S) -> uint8;
JAFGEXTERNALCORE_API auto IsInteger32(const FString& S) -> bool;
JAFGEXTERNALCORE_API auto ToInteger32(const FString& S) -> int32_t;
JAFGEXTERNALCORE_API auto IsUInteger32(const FString& S) -> bool;
JAFGEXTERNALCORE_API auto ToUInteger32(const FString& S) -> uint32_t;

JAFGEXTERNALCORE_API bool IsByte_RangeCheck(const FString& S);
JAFGEXTERNALCORE_API bool IsInteger32_RangeCheck(const FString& S);
JAFGEXTERNALCORE_API bool IsUInteger32_RangeCheck(const FString& S);

JAFGEXTERNALCORE_API bool AssumedInteger32_RangeCheck(const FString& S);
JAFGEXTERNALCORE_API bool AssumedUInteger32_RangeCheck(const FString& S);

}

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UShippedWorldChatCommandRegistry : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UShippedWorldChatCommandRegistry();

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Deinitialize(void) override;
    // ~UWorldSubsystem implementation

    bool RegisterCommand(const FChatCommandObject& InObject);
    bool UnregisterCommand(const FChatCommand& InCommand);
    bool IsCommandRegistered(const FChatCommand& InCommand) const;

    bool RegisterRemoteCommand(const FChatCommandObject& InObject);
    bool UnregisterRemoteCommand(const FChatCommand& Command);
    bool IsRemoteCommandRegistered(const FChatCommand& Command) const;

    bool IsAnyCommandRegistered(const FChatCommand& Command) const;
    bool GetAnyCommandObj(const FChatCommand& InCommand, const FChatCommandObject*& OutObj) const;

    FChatCommand SmartPrefix(const FChatCommand& InCommand) const;

    bool RegisterSyntax(const FSyntaxIdentifier& InIdent, FSyntaxCallback InCallback);
    bool UnregisterSyntax(const FSyntaxIdentifier& InIdent);
    bool IsSyntaxRegistered(const FSyntaxIdentifier& InIdent) const;

    void ExecuteCommand(UChatComponent* InCallee, const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const;
    /** Shortcut if local execution was targeted. */
    void ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const;
    void ExecuteCommand(const FChatCommand& Command, FChatCommandParams& Params) const;

    auto GetLocalChatComponent(void) const -> UChatComponent*;
    auto GetSafeLocalChatComponent(void) const -> UChatComponent*;

    FORCEINLINE auto GetRegisteredCommands(void) const -> const TMap<FChatCommand, FChatCommandObject>& { return this->RegisteredCommands; }
    FORCEINLINE auto GetRegisteredRemoteCommands(void) const -> const TMap<FChatCommand, FChatCommandObject>& { return this->RegisteredRemoteCommands; }

    auto GetCommandsThatStartWith(const FString& InBegin, const bool bInCutPrefix = false, const int32 InMax = 20) const -> TArray<FString>;

private:

    TMap<FSyntaxIdentifier, FSyntaxCallback>    RegisteredSyntax;
    TMap<FChatCommand,      FChatCommandObject> RegisteredCommands;
    TMap<FChatCommand,      FChatCommandObject> RegisteredRemoteCommands;
};

#undef DEFAULT_SYNTAX_RET_LIMIT
