// Copyright 2024 mzoesch. All rights reserved.

#include "ClientCommandSubsystem.h"

#include "ChatComponent.h"
#include "ChatMenu.h"
#include "CommonNetworkStatics.h"
#include "Definitions.h"

#define OWNING_CHAT_COMPONENT                                                                       \
    GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetComponentByClass<UChatComponent>()
#define DECLARE_CLIENT_COMMAND(Name, Callback)                                                      \
    {                                                                                               \
        const FClientCommand Command =                                                              \
            FString::Printf(                                                                        \
            TEXT("%s%s"),                                                                           \
                *CommandStatics::ClientCommandPrefix, TEXT(Name)                                    \
            );                                                                                      \
        this->ClientCommands.Add(Command,                                                           \
            [this]                                                                                  \
            (const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) \
        {                                                                                           \
            this->Callback(InArgs, OutReturnCode, OutResponse);                                     \
        });                                                                                         \
    }

void UClientCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->ClientCommands.Empty();

    this->InitializeAllCommands();

    return;
}

bool UClientCommandSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        return false;
    }
#endif /* WITH_EDITOR */

    return UNetStatics::IsSafeDedicatedServer(Outer) == false;
}

void UClientCommandSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

bool UClientCommandSubsystem::IsRegisteredClientCommand(const FText& StdIn) const
{
    return this->IsRegisteredClientCommand(CommandStatics::GetCommandWithClientPrefix(StdIn));
}

bool UClientCommandSubsystem::IsRegisteredClientCommand(const FClientCommand& Command) const
{
    if (Command.IsEmpty())
    {
        return false;
    }

    return this->ClientCommands.Contains(CommandStatics::SafePrefixClientCommand(Command));
}

void UClientCommandSubsystem::ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    TArray<FString> Args;
    FClientCommand Command = CommandStatics::GetCommandWithArgs(StdIn, Args);
    Command = CommandStatics::SafePrefixClientCommand(Command);
    this->ExecuteCommand(Command, Args, OutReturnCode, OutResponse);

    return;
}

void UClientCommandSubsystem::ExecuteCommand(const FClientCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Executing client command: [%s].", *Command)

    if (this->IsRegisteredClientCommand(Command) == false)
    {
        OutReturnCode = ECommandReturnCodes::Unknown;
        OutResponse   = L"";

        return;
    }

    this->ClientCommands[Command](Args, OutReturnCode, OutResponse);

    return;
}

void UClientCommandSubsystem::InitializeAllCommands(void)
{
    DECLARE_CLIENT_COMMAND("help", OnHelpCommand)
    DECLARE_CLIENT_COMMAND("clear", OnChatClearCommand)

    return;
}

void UClientCommandSubsystem::OnHelpCommand(const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    for (const TPair<FClientCommand, FClientCommandCallback>& Pair : this->ClientCommands)
    {
        OWNING_CHAT_COMPONENT->AddMessageToChatLog(ChatStatics::InternalName, FText::FromString(Pair.Key));
    }

    OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
    OutResponse   = L"";

    return;
}

void UClientCommandSubsystem::OnChatClearCommand(const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    OWNING_CHAT_COMPONENT->GetSafeChatMenu()->ClearAllChatEntries();

    OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
    OutResponse   = L"";

    return;
}

#undef OWNING_CHAT_COMPONENT
#undef DECLARE_CLIENT_COMMAND
