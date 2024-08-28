// Copyright 2024 mzoesch. All rights reserved.

#include "Commands/ShippedWorldChatCommands.h"
#include "Chat/ChatComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "System/VoxelSubsystem.h"

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

FString LexToString(const EChatCommandSyntax::Type& InType)
{
    switch (InType)
    {
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
    case EChatCommandSyntax::Integer:
    {
        return TEXT("Integer");
    }
    case EChatCommandSyntax::Custom:
    {
        return TEXT("Custom");
    }
    default:
    {
        return TEXT("Unknown");
    }
    }
}

bool CommandStatics::Syntax::ParseArgument(
    const TArray<FString>& InArgs,
    const EChatCommandSyntax::Type& InArgSyntax,
    FString& OutArgStringRepresentation,
    FArgCursor& MovableArgCursor,
    bool& bBadInput
)
{
    bBadInput = false;

    if (MovableArgCursor >= InArgs.Num())
    {
        return false;
    }

    if (InArgSyntax == EChatCommandSyntax::Any)
    {
        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (InArgSyntax == EChatCommandSyntax::SharpSharpAny)
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

    if (InArgSyntax == EChatCommandSyntax::PlayerName)
    {
        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (InArgSyntax == EChatCommandSyntax::Accumulated)
    {
        OutArgStringRepresentation = InArgs[MovableArgCursor];
        ++MovableArgCursor;
        return true;
    }

    if (InArgSyntax == EChatCommandSyntax::Integer)
    {
        if (InArgs[MovableArgCursor].IsNumeric())
        {
            OutArgStringRepresentation = InArgs[MovableArgCursor];
            ++MovableArgCursor;
            return true;
        }

        bBadInput = true;

        return false;
    }

    return false;
}

void CommandStatics::Syntax::GetAllAvailableInputsForSyntax(
    const UWorld& InContext,
    const EChatCommandSyntax::Type& InSyntax,
    const TArray<FString>& InArgs,
    const FArgCursor InArgCursor,
    TArray<FString>& OutPossibleInputs,
    const int32 InLimit /* = 20 */
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
    if (InArgs)
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

    // for (const FItemMask& Mask : Subsystem->GetItemMasks())
    // {
    //     if (OutPossibleInputs.Num() >= InLimit)
    //     {
    //         break;
    //     }
    //
    //     continue;
    // }

    OutName.Sort( [] (const FString& A, const FString& B) -> bool { return A < B; } );
    OutNamespaceName.Sort( [] (const FString& A, const FString& B) -> bool { return A < B; } );

    OutPossibleInputs.Append(OutName);
    OutPossibleInputs.Append(OutNamespaceName);

#undef OUT_NUM

    return;
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
    bool bClientCommand;
    if (CommandStatics::DoesCommandStartWithSpecificCommandType(InCommand, bClientCommand))
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
