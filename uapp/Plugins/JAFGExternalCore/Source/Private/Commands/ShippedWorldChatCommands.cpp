// Copyright 2024 mzoesch. All rights reserved.

#include "Commands/ShippedWorldChatCommands.h"
#include "Chat/ChatComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "System/VoxelSubsystem.h"
#include "UObject/FastReferenceCollector.h"

#define SPECIFIC_SYNTAX_CODE_CHECK()                    \
    checkCode(                                          \
        if (InArgs == nullptr)                          \
        {                                               \
            check( InArgCursor == nullptr )             \
        }                                               \
        else                                            \
        {                                               \
            check( InArgCursor != nullptr )             \
            check( InArgs->IsValidIndex(*InArgCursor) ) \
        }                                               \
    )

EChatCommandSyntax::Type EChatCommandSyntax::Redirect(const EChatCommandSyntax::Type& InType)
{
    if (InType == EChatCommandSyntax::Integer)
    {
        return EChatCommandSyntax::Integer32;
    }

    if (InType == EChatCommandSyntax::AccAmount)
    {
        return EChatCommandSyntax::UInteger32;
    }

    return InType;
}

FString LexToString(const EChatCommandSyntax::Type& InType)
{
    switch (InType)
    {

    //////////////////////////////////////////////////////////////////////////
    // Typedefs
    //////////////////////////////////////////////////////////////////////////

    case EChatCommandSyntax::Any:
    {
        return TEXT("Any");
    }
    case EChatCommandSyntax::SharpSharpAny:
    {
        return TEXT("##Any");
    }
    case EChatCommandSyntax::PlayerName:
    {
        return TEXT("PlayerName");
    }
    case EChatCommandSyntax::Location:
    {
        return TEXT("Location");
    }
    case EChatCommandSyntax::Accumulated:
    {
        return TEXT("Accumulated");
    }
    case EChatCommandSyntax::Voxel:
    {
        return TEXT("Voxel");
    }
    case EChatCommandSyntax::Item:
    {
        return TEXT("Item");
    }
    case EChatCommandSyntax::Byte:
    {
        return TEXT("Byte");
    }
    case EChatCommandSyntax::Integer32:
    {
        return TEXT("Integer32");
    }
    case EChatCommandSyntax::UInteger32:
    {
        return TEXT("UInteger32");
    }

    //////////////////////////////////////////////////////////////////////////
    // Redirected typedefs
    //////////////////////////////////////////////////////////////////////////

    case EChatCommandSyntax::Integer:
    {
        return TEXT("Integer");
    }
    case EChatCommandSyntax::AccAmount:
    {
        return TEXT("AccAmount");
    }

    //////////////////////////////////////////////////////////////////////////
    // Subsystem redirections
    //////////////////////////////////////////////////////////////////////////

    case EChatCommandSyntax::Custom:
    {
        return TEXT("SubsystemRedirected");
    }
    default:
    {
        if (EChatCommandSyntax::IsSubsystemRedirected(InType))
        {
            return TEXT("__SubsystemRedirected");
        }

        return TEXT("__Unspecified");
    }
    }
}

bool CommandStatics::Syntax::ParseArgument(
    const UWorld& InContext,
    const TArray<FString>& InArgs,
    const EChatCommandSyntax::Type& InArgSyntax,
    FString& OutArgStringRepresentation,
    FArgCursor& MovableArgCursor,
    bool& bBadInput
)
{
#define BAD_INPUT_RET() \
    bBadInput = true; return false;

    const EChatCommandSyntax::Type RedirectedArgSyntax = EChatCommandSyntax::Redirect(InArgSyntax);

    bBadInput = false;

    if (MovableArgCursor >= InArgs.Num())
    {
        return false;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Any)
    {
        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::SharpSharpAny)
    {
        for (int32 i = MovableArgCursor; i < InArgs.Num(); ++i)
        {
            OutArgStringRepresentation += InArgs[i];
            if (i < InArgs.Num() - 1)
            {
                OutArgStringRepresentation += TEXT(" ");
            }
        }
        MovableArgCursor = InArgs.Num();
        return true;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::PlayerName)
    {
        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Accumulated)
    {
        if (CommandStatics::Syntax::IsUInteger32_RangeCheck(InArgs[MovableArgCursor]))
        {
            FString Name; FString Namespace;
            if (InContext.GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetMaybeUndefinedAccumulated(
                CommandStatics::Syntax::ToUInteger32(InArgs[MovableArgCursor]),
                Name,
                Namespace
            ) == false)
            {
                BAD_INPUT_RET()
            }

            OutArgStringRepresentation = ::Accumulated::Join(Namespace, Name);
            ++MovableArgCursor;
            return true;
        }

        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Voxel)
    {
        jveryRelaxedCheck( false && "Voxel" )
        return false;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Item)
    {
        jveryRelaxedCheck( false && "Item" )
        return false;
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Byte)
    {
        if (CommandStatics::Syntax::IsByte_RangeCheck(InArgs[MovableArgCursor]))
        {
            OutArgStringRepresentation = FString::Printf(TEXT("%u"), CommandStatics::Syntax::ToByte(InArgs[MovableArgCursor]));
            return true;
        }

        BAD_INPUT_RET()
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::Integer32)
    {
        if (CommandStatics::Syntax::IsInteger32_RangeCheck(InArgs[MovableArgCursor]))
        {
            OutArgStringRepresentation = InArgs[MovableArgCursor];
            ++MovableArgCursor;
            return true;
        }

        BAD_INPUT_RET()
    }

    if (RedirectedArgSyntax == EChatCommandSyntax::UInteger32)
    {
        if (CommandStatics::Syntax::IsUInteger32_RangeCheck(InArgs[MovableArgCursor]))
        {
            OutArgStringRepresentation = InArgs[MovableArgCursor];
            ++MovableArgCursor;
            return true;
        }

        BAD_INPUT_RET()
    }

    if (EChatCommandSyntax::IsSubsystemRedirected(RedirectedArgSyntax))
    {
        jveryRelaxedCheck( false && "SubsystemRedirected" )
    }

    return false;
#undef BAD_INPUT_RET
}

void CommandStatics::Syntax::GetAllAvailableInputsForSyntax(
    const UWorld& InContext,
    const EChatCommandSyntax::Type& InSyntax,
    const TArray<FString>& InArgs,
    const FArgCursor InArgCursor,
    TArray<FString>& OutPossibleInputs,
    const int32 InLimit /* = DEFAULT_SYNTAX_RET_LIMIT */
)
{
    check( OutPossibleInputs.IsEmpty() )

    if (InSyntax == EChatCommandSyntax::Any || InSyntax == EChatCommandSyntax::SharpSharpAny)
    {
        return;
    }

    const bool bOOB = InArgCursor >= InArgs.Num();

    if (InSyntax == EChatCommandSyntax::PlayerName)
    {
        CommandStatics::Syntax::GetAllAvailableInputsForSyntax_PlayerName(
            InContext,
            OutPossibleInputs,
            bOOB ? nullptr : &InArgs,
            bOOB ? nullptr : &InArgCursor,
            InLimit
        );

        return;
    }

    if (InSyntax == EChatCommandSyntax::Accumulated)
    {
        CommandStatics::Syntax::GetAllAvailableInputsForSyntax_Accumulated(
            InContext,
            OutPossibleInputs,
            bOOB ? nullptr : &InArgs,
            bOOB ? nullptr : &InArgCursor,
            InLimit
        );

        return;
    }

    if (InSyntax == EChatCommandSyntax::Integer || InSyntax == EChatCommandSyntax::Accumulated)
    {
        return;
    }

    return;
}

void CommandStatics::Syntax::GetAllAvailableInputsForSyntax_PlayerName(
    const UWorld& InContext,
    TArray<FString>& OutPossibleInputs,
    const TArray<FString>* InArgs /* = nullptr */,
    const FArgCursor* InArgCursor /* = nullptr */,
    const int32 InLimit /* DEFAULT_SYNTAX_RESULT_LIMIT */
)
{
    SPECIFIC_SYNTAX_CODE_CHECK()

    for (const APlayerState* State : InContext.GetGameState()->PlayerArray)
    {
        if (OutPossibleInputs.Num() >= InLimit)
        {
            break;
        }

        if (State == nullptr)
        {
            continue;
        }

        if (InArgs)
        {
            if (State->GetPlayerName().StartsWith((*InArgs)[*InArgCursor]))
            {
                OutPossibleInputs.Emplace(State->GetPlayerName());
            }
        }
        else
        {
            OutPossibleInputs.Emplace(State->GetPlayerName());
        }

        continue;
    }

    return;
}

void CommandStatics::Syntax::GetAllAvailableInputsForSyntax_Accumulated(
    const UWorld& InContext,
    TArray<FString>& OutPossibleInputs,
    const TArray<FString>* InArgs /* = nullptr */,
    const FArgCursor* InArgCursor /* = nullptr */,
    const int32 InLimit /* DEFAULT_SYNTAX_RESULT_LIMIT */
)
{
    SPECIFIC_SYNTAX_CODE_CHECK()

#define OUT_NUM                              \
    (OutNamespaceName.Num() + OutName.Num())

    TArray<FString> OutName;
    TArray<FString> OutNamespaceName;

    bool    bCopiedArgs        = false;
    bool    bFoundNamespaceSep = false;
    FString Namespace          = TEXT("");
    FString Name               = TEXT("");
    if (InArgs && CommandStatics::Syntax::IsUInteger32_RangeCheck((*InArgs)[*InArgCursor]))
    {
        if (InContext.GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetMaybeUndefinedAccumulated(
            CommandStatics::Syntax::ToUInteger32((*InArgs)[*InArgCursor]),
            Name,
            Namespace
        ))
        {
            OutPossibleInputs.Emplace(Accumulated::Join(Namespace, Name));
        }
        return;
    }
    else if (InArgs)
    {
        if (Accumulated::UnrealSplit((*InArgs)[*InArgCursor], Namespace, Name) == false)
        {
            /*
             * We do not know what we are searching for. A specific accumulated by name or all accumulates inside
             * a namespace. Therefore, we search both for matching names and namespaces.
             */
            bCopiedArgs = true;
            Namespace   = (*InArgs)[*InArgCursor];
            Name        = (*InArgs)[*InArgCursor];
        }

        bFoundNamespaceSep = (*InArgs)[*InArgCursor].Contains(Accumulated::NamespaceToNameSplitter);
    }

    const UVoxelSubsystem* Subsystem = InContext.GetGameInstance()->GetSubsystem<UVoxelSubsystem>();

    for (const FVoxelMask& Mask : Subsystem->GetVoxelMasks())
    {
        if (OUT_NUM >= InLimit)
        {
            break;
        }

        if (Namespace.IsEmpty() && Name.IsEmpty())
        {
            if (bFoundNamespaceSep)
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }
            else
            {
                OutName.Emplace(Mask.Name);
            }

            continue;
        }

        if (Name.IsEmpty())
        {
            check( Namespace.IsEmpty() == false && Name.IsEmpty() )

            /* Always add namespace as prefix for out. We are looking at every accumulated inside a space. */
            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase))
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }

            continue;
        }

        if (Namespace.IsEmpty())
        {
            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase) && Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
            {
                check( Namespace.IsEmpty() && Name.IsEmpty() == false )

                if (bFoundNamespaceSep)
                {
                    OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
                }
                else
                {
                    OutName.Emplace(Mask.Name);
                }
            }

            continue;
        }

        if (bCopiedArgs)
        {
            check( bFoundNamespaceSep == false )
            check( Namespace.IsEmpty() == false && Name.IsEmpty() == false )

            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase))
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }

            if (Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
            {
                OutName.Emplace(Mask.Name);
            }

            continue;
        }

        if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase) && Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
        {
            check( Namespace.IsEmpty() == false && Name.IsEmpty() == false )

            if (bFoundNamespaceSep)
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }
            else
            {
                OutName.Emplace(Mask.Name);
            }
        }

        continue;
    }

    for (const FItemMask& Mask : Subsystem->GetItemMasks())
    {
        if (OUT_NUM >= InLimit)
        {
            break;
        }

        if (Namespace.IsEmpty() && Name.IsEmpty())
        {
            if (bFoundNamespaceSep)
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }
            else
            {
                OutName.Emplace(Mask.Name);
            }

            continue;
        }

        if (Name.IsEmpty())
        {
            check( Namespace.IsEmpty() == false && Name.IsEmpty() )

            /* Always add namespace as prefix for out. We are looking at every accumulated inside a space. */
            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase))
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }

            continue;
        }

        if (Namespace.IsEmpty())
        {
            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase) && Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
            {
                check( Namespace.IsEmpty() && Name.IsEmpty() == false )

                if (bFoundNamespaceSep)
                {
                    OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
                }
                else
                {
                    OutName.Emplace(Mask.Name);
                }
            }

            continue;
        }

        if (bCopiedArgs)
        {
            check( bFoundNamespaceSep == false )
            check( Namespace.IsEmpty() == false && Name.IsEmpty() == false )

            if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase))
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }

            if (Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
            {
                OutName.Emplace(Mask.Name);
            }

            continue;
        }

        if (Mask.Namespace.StartsWith(Namespace, ESearchCase::IgnoreCase) && Mask.Name.StartsWith(Name, ESearchCase::IgnoreCase))
        {
            check( Namespace.IsEmpty() == false && Name.IsEmpty() == false )

            if (bFoundNamespaceSep)
            {
                OutNamespaceName.Emplace(Accumulated::Join(Mask.Namespace, Mask.Name));
            }
            else
            {
                OutName.Emplace(Mask.Name);
            }
        }

        continue;
    }

    OutName.Sort( [] (const FString& A, const FString& B) -> bool { return A < B; } );
    OutNamespaceName.Sort( [] (const FString& A, const FString& B) -> bool { return A < B; } );

    OutPossibleInputs.Append(OutName);
    OutPossibleInputs.Append(OutNamespaceName);

#undef OUT_NUM

    return;
}

bool CommandStatics::Syntax::IsNumeric(const FString& S)
{
    return S.IsNumeric();
}

bool CommandStatics::Syntax::IsByte(const FString& S)
{
    if (CommandStatics::Syntax::IsUInteger32(S))
    {
        /* We do not care about the range. */
        return true;
    }

    FString Lowered = S.ToLower(); Lowered.TrimStartAndEndInline();

    if (Lowered.IsEmpty() || Lowered.Len() > 4)
    {
        return false;
    }

    if (Lowered.Len() == 4)
    {
        if (Lowered[0] != TEXT('0') || Lowered[1] != TEXT('x'))
        {
            return false;
        }

        return FChar::IsHexDigit(Lowered[2]) && FChar::IsHexDigit(Lowered[3]);
    }

    if (Lowered.Len() == 3)
    {
        if (Lowered[0] != TEXT('0') || Lowered[1] != TEXT('x'))
        {
            return false;
        }

        return FChar::IsHexDigit(Lowered[2]);
    }

    if (Lowered.Len() == 2)
    {
       return FChar::IsHexDigit(Lowered[0]) && FChar::IsHexDigit(Lowered[1]);
    }

    if (Lowered.Len() == 1)
    {
        return FChar::IsHexDigit(Lowered[0]);
    }

    checkNoEntry()

    return false;
}

uint8 CommandStatics::Syntax::ToByte(const FString& S)
{
#define CHAR_TO_BYTE(InChar) \
    static_cast<uint8>(FChar::IsDigit(InChar) ? InChar - TEXT('0') : InChar - TEXT('a') + 10)

    check( CommandStatics::Syntax::IsByte(S) )

    if (CommandStatics::Syntax::IsUInteger32(S))
    {
        check( CommandStatics::Syntax::ToUInteger32(S) <= TNumericLimits<uint8>::Max() )
        return static_cast<uint8>(CommandStatics::Syntax::ToUInteger32(S));
    }

    FString Lowered = S.ToLower(); Lowered.TrimStartAndEndInline();

    if (Lowered.Len() == 4)
    {
        return (CHAR_TO_BYTE(Lowered[2]) << 4) + CHAR_TO_BYTE(Lowered[3]);
    }

    if (Lowered.Len() == 3)
    {
        return CHAR_TO_BYTE(Lowered[2]);
    }

    if (Lowered.Len() == 2)
    {
        return (CHAR_TO_BYTE(Lowered[0]) << 4) + CHAR_TO_BYTE(Lowered[1]);
    }

    if (Lowered.Len() == 1)
    {
        return CHAR_TO_BYTE(Lowered[0]);
    }

    checkNoEntry()
    return 0b0;

#undef CHAR_TO_BYTE
}

bool CommandStatics::Syntax::IsInteger32(const FString& S)
{
    if (S.IsEmpty())
    {
        return false;
    }

    bool bFirst = true;
    for (const TCHAR& Char : S)
    {
        if (bFirst)
        {
            if (Char == '-' || Char == '+')
            {
                bFirst = false;
                continue;
            }

            bFirst = false;
        }

        if (FChar::IsDigit(Char) == false)
        {
            return false;
        }

        continue;
    }

    return true;
}

int32_t CommandStatics::Syntax::ToInteger32(const FString& S)
{
    check( CommandStatics::Syntax::IsInteger32_RangeCheck(S) )

    /*
     * This is a super edgy edge-case. We assert here to come back in the future if this really
     * fails to test this function. We should probably do some unit tests for this - lol.
     * But theoretically, this if approach with the signed 64-Bit Integer here should work? If not, then maybe the
     * user is just fucked and should buy a newer platform to run this game...
     */
    static_assert( sizeof(int32_t) == sizeof(int32) );

    if constexpr (sizeof(int32) < sizeof(int32_t))
    {
        return FCString::Atoi64(*S);
    }

    return FCString::Atoi(*S);
}

bool CommandStatics::Syntax::IsUInteger32(const FString& S)
{
    if (S.IsEmpty())
    {
        return false;
    }

    bool bFirst = true;
    for (const TCHAR& Char : S)
    {
        if (bFirst)
        {
            if (/* Checking for unsigned integer - no minus allowed. */ Char == '+')
            {
                bFirst = false;
                continue;
            }

            bFirst = false;
        }

        if (FChar::IsDigit(Char) == false)
        {
            return false;
        }

        continue;
    }

    return true;
}

uint32_t CommandStatics::Syntax::ToUInteger32(const FString& S)
{
    check( CommandStatics::Syntax::IsUInteger32_RangeCheck(S) )
    return static_cast<uint32_t>(FCString::Atoi64(*S));
}

bool CommandStatics::Syntax::IsByte_RangeCheck(const FString& S)
{
    if (CommandStatics::Syntax::IsByte(S) == false)
    {
        return false;
    }

    if (CommandStatics::Syntax::IsUInteger32(S))
    {
        return CommandStatics::Syntax::ToUInteger32(S) <= TNumericLimits<uint8>::Max();
    }

    return true;
}

bool CommandStatics::Syntax::IsInteger32_RangeCheck(const FString& S)
{
    if (CommandStatics::Syntax::IsInteger32(S) == false)
    {
        return false;
    }

    const int64 Value = FCString::Atoi64(*S);
    return Value >= TNumericLimits<int32_t>::Min() && Value <= TNumericLimits<int32_t>::Max();
}

bool CommandStatics::Syntax::IsUInteger32_RangeCheck(const FString& S)
{
    if (CommandStatics::Syntax::IsUInteger32(S) == false)
    {
        return false;
    }

    const int64 Value = FCString::Atoi64(*S);
    return Value >= TNumericLimits<uint32_t>::Min() && Value <= TNumericLimits<uint32_t>::Max();
}

bool CommandStatics::Syntax::AssumedInteger32_RangeCheck(const FString& S)
{
    check( CommandStatics::Syntax::IsInteger32(S) )
    const int64 Value = FCString::Atoi64(*S);
    return Value >= TNumericLimits<int32_t>::Min() && Value <= TNumericLimits<int32_t>::Max();
}

bool CommandStatics::Syntax::AssumedUInteger32_RangeCheck(const FString& S)
{
    check( CommandStatics::Syntax::IsUInteger32(S) )
    const uint64 Value = FCString::Atoi64(*S);
    return Value <= TNumericLimits<uint32_t>::Max();
}

UShippedWorldChatCommandRegistry::UShippedWorldChatCommandRegistry(void) : Super()
{
    return;
}

void UShippedWorldChatCommandRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->RegisteredSyntax.Empty();
    this->RegisteredCommands.Empty();
    this->RegisteredRemoteCommands.Empty();

    return;
}

bool UShippedWorldChatCommandRegistry::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}

void UShippedWorldChatCommandRegistry::Deinitialize(void)
{
    Super::Deinitialize();
}

bool UShippedWorldChatCommandRegistry::RegisterCommand(const FChatCommandObject& InObject)
{
    if (this->IsCommandRegistered(InObject.Command))
    {
        return false;
    }

    this->RegisteredCommands.Emplace(InObject.Command, InObject);

    return true;
}

bool UShippedWorldChatCommandRegistry::UnregisterCommand(const FChatCommand& InCommand)
{
    if (this->IsCommandRegistered(InCommand) == false)
    {
        return false;
    }

    this->RegisteredCommands.Remove(InCommand);

    return true;
}

bool UShippedWorldChatCommandRegistry::IsCommandRegistered(const FChatCommand& InCommand) const
{
    return this->RegisteredCommands.Contains(InCommand);
}

bool UShippedWorldChatCommandRegistry::RegisterRemoteCommand(const FChatCommandObject& InObject)
{
    if (this->IsRemoteCommandRegistered(InObject.Command))
    {
        return false;
    }

    this->RegisteredRemoteCommands.Emplace(InObject.Command, InObject);

    return true;
}

bool UShippedWorldChatCommandRegistry::UnregisterRemoteCommand(const FChatCommand& Command)
{
    if (this->IsRemoteCommandRegistered(Command) == false)
    {
        return false;
    }

    this->RegisteredRemoteCommands.Remove(Command);

    return true;
}

bool UShippedWorldChatCommandRegistry::IsRemoteCommandRegistered(const FChatCommand& Command) const
{
    return this->RegisteredRemoteCommands.Contains(Command);
}

bool UShippedWorldChatCommandRegistry::IsAnyCommandRegistered(const FChatCommand& Command) const
{
    if (this->IsCommandRegistered(Command))
    {
        return true;
    }

    if (this->IsRemoteCommandRegistered(Command))
    {
        return true;
    }

    return false;
}

bool UShippedWorldChatCommandRegistry::GetAnyCommandObj(const FChatCommand& InCommand, const FChatCommandObject*& OutObj) const
{
    if (const FChatCommandObject* Command = this->RegisteredCommands.Find(InCommand); Command)
    {
        OutObj = Command;
        return true;
    }

    if (const FChatCommandObject* Command = this->RegisteredRemoteCommands.Find(InCommand); Command)
    {
        OutObj = Command;
        return true;
    }

    return false;
}

FChatCommand UShippedWorldChatCommandRegistry::SmartPrefix(const FChatCommand& InCommand) const
{
    if (bool bClientCommand; CommandStatics::DoesCommandStartWithSpecificCommandType(InCommand, bClientCommand))
    {
        return InCommand;
    }

    if (this->IsAnyCommandRegistered(CommandStatics::SafePrefixClientCommand(InCommand)))
    {
        return CommandStatics::SafePrefixClientCommand(InCommand);
    }

    if (this->IsAnyCommandRegistered(CommandStatics::SafePrefixServerCommand(InCommand)))
    {
        return CommandStatics::SafePrefixServerCommand(InCommand);
    }

    return FString();
}

bool UShippedWorldChatCommandRegistry::RegisterSyntax(const FSyntaxIdentifier& InIdent, FSyntaxCallback InCallback)
{
    if (this->IsSyntaxRegistered(InIdent))
    {
        return false;
    }

    this->RegisteredSyntax.Emplace(InIdent, InCallback);

    return true;
}

bool UShippedWorldChatCommandRegistry::UnregisterSyntax(const FSyntaxIdentifier& InIdent)
{
    if (this->IsSyntaxRegistered(InIdent) == false)
    {
        return false;
    }

    this->RegisteredSyntax.Remove(InIdent);

    return true;
}

bool UShippedWorldChatCommandRegistry::IsSyntaxRegistered(const FSyntaxIdentifier& InIdent) const
{
    return this->RegisteredSyntax.Contains(InIdent);
}

void UShippedWorldChatCommandRegistry::ExecuteCommand(UChatComponent* InCallee, const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    check( InCallee )

    TArray<FString>    Args;
    const FChatCommand Command = CommandStatics::GetCommandWithArgs(StdIn, Args);
    FChatCommandParams Params  = FChatCommandParams(InCallee, Args, OutReturnCode, OutResponse);

    if (bool bClientCommand; CommandStatics::DoesStdInStartWithSpecificCommandType(StdIn, bClientCommand))
    {
        if (bClientCommand)
        {
            this->ExecuteCommand(Command, Params);
        }
        /* For listen servers or standalone. */
        if (this->IsCommandRegistered(Command))
        {
            this->ExecuteCommand(Command, Params);
        }
        else if (this->IsRemoteCommandRegistered(Command))
        {
            InCallee->QueueCommandForServer(StdIn);
        }
        else
        {
            OutReturnCode = ECommandReturnCode::Unknown;
        }
    }
    else
    {
        if (this->IsCommandRegistered(CommandStatics::SafePrefixClientCommand(Command)))
        {
            this->ExecuteCommand(CommandStatics::SafePrefixClientCommand(Command), Params);
        }
        /* For listen server or standalone. */
        else if (this->IsCommandRegistered(CommandStatics::SafePrefixServerCommand(Command)))
        {
            this->ExecuteCommand(CommandStatics::SafePrefixServerCommand(Command), Params);
        }
        else if (this->IsRemoteCommandRegistered(CommandStatics::SafePrefixServerCommand(Command)))
        {
            InCallee->QueueCommandForServer(StdIn);
        }
        else
        {
            OutReturnCode = ECommandReturnCode::Unknown;
        }
    }

    return;
}

void UShippedWorldChatCommandRegistry::ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    this->ExecuteCommand(this->GetSafeLocalChatComponent(), StdIn, OutReturnCode, OutResponse);
}

void UShippedWorldChatCommandRegistry::ExecuteCommand(const FChatCommand& Command, FChatCommandParams& Params) const
{
    if (this->IsCommandRegistered(Command))
    {
        this->RegisteredCommands[Command].Callback(Params);
    }
    else
    {
        Params.OutReturnCode = ECommandReturnCode::Unknown;
    }

    return;
}

UChatComponent* UShippedWorldChatCommandRegistry::GetLocalChatComponent(void) const
{
    return GEngine ? GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetComponentByClass<UChatComponent>() : nullptr;
}

UChatComponent* UShippedWorldChatCommandRegistry::GetSafeLocalChatComponent(void) const
{
    if (UChatComponent* ChatComponent = this->GetLocalChatComponent())
    {
        return ChatComponent;
    }

    LOG_FATAL(LogJAFGChat, "Failed to get the local chat component.");

    return nullptr;
}

TArray<FString> UShippedWorldChatCommandRegistry::GetCommandsThatStartWith(const FString& InBegin, const bool bInCutPrefix /* = false */, const int32 InMax /* = 20 */) const
{
    TArray<FString> Out;

    if (InBegin.IsEmpty())
    {
        for (const TTuple<FString, FChatCommandObject>& Command : this->RegisteredCommands)
        {
            if (Out.Num() >= InMax)
            {
                break;
            }

            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);

            continue;
        }

        for (const TTuple<FString, FChatCommandObject>& Command : this->RegisteredRemoteCommands)
        {
            if (Out.Num() >= InMax)
            {
                break;
            }

            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);

            continue;
        }

        return Out;
    }

    for (const TTuple<FString, FChatCommandObject>& Command : this->RegisteredCommands)
    {
        if (Out.Num() >= InMax)
        {
            break;
        }

        if (Command.Key.StartsWith(InBegin))
        {
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else if (Command.Key.StartsWith(CommandStatics::SafePrefixClientCommand(InBegin)))
        {
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else if (Command.Key.StartsWith(CommandStatics::SafePrefixServerCommand(InBegin)))
        {
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else
        {
            continue;
        }

        continue;
    }

    for (const TTuple<FString, FChatCommandObject>& Command : this->RegisteredRemoteCommands)
    {
        if (Out.Num() >= InMax)
        {
            break;
        }

        if (Command.Key.StartsWith(InBegin))
        {
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else if (Command.Key.StartsWith(CommandStatics::SafePrefixClientCommand(InBegin)))
        {
            checkNoEntry()
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else if (Command.Key.StartsWith(CommandStatics::SafePrefixServerCommand(InBegin)))
        {
            Out.Emplace(bInCutPrefix ? Command.Key.RightChop(CommandStatics::PlatformCommandPrefixLength) : Command.Key);
        }
        else
        {
            continue;
        }

        continue;
    }

    return Out;
}

#undef SPECIFIC_SYNTAX_CODE_CHECK
