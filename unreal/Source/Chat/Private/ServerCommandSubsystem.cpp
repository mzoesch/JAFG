// Copyright 2024 mzoesch. All rights reserved.

#include "ServerCommandSubsystem.h"

#include "ChatComponent.h"
#include "CommonNetworkStatics.h"
#include "Definitions.h"

void UServerCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->ServerCommands.Empty();

    this->InitializeAllCommands();

    return;
}

bool UServerCommandSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

    return UNetStatics::IsSafeServer(Outer);
}

void UServerCommandSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

bool UServerCommandSubsystem::IsRegisteredCommand(const FText& StdIn) const
{
    return this->IsRegisteredCommand(CommandStatics::GetCommandWithServerPrefix(StdIn));
}

bool UServerCommandSubsystem::IsRegisteredCommand(const FServerCommand& Command) const
{
    if (Command.IsEmpty())
    {
        return false;
    }

    return this->ServerCommands.Contains(CommandStatics::SafePrefixServerCommand(Command));
}

void UServerCommandSubsystem::ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    TArray<FString> Args;
    FServerCommand Command = CommandStatics::GetCommandWithArgs(StdIn, Args);
    Command = CommandStatics::SafePrefixServerCommand(Command);
    this->ExecuteCommand(Command, Args, OutReturnCode, OutResponse);

    return;
}

void UServerCommandSubsystem::ExecuteCommand(const FServerCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Executing server command: [%s].", *Command)

    if (this->IsRegisteredCommand(Command) == false)
    {
        OutReturnCode = ECommandReturnCodes::Unknown;
        OutResponse   = L"";

        return;
    }

    this->ServerCommands[Command](Args, OutReturnCode, OutResponse);

    return;
}

void UServerCommandSubsystem::InitializeAllCommands(void)
{
    // Broadcast
    //////////////////////////////////////////////////////////////////////////
    {
        const FServerCommand Command = TEXT("sv_broadcast");
        this->ServerCommands.Add(Command, [this] (const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        {
            this->OnBroadcastCommand(InArgs, OutReturnCode, OutResponse);
        });
    }
}

void UServerCommandSubsystem::OnBroadcastCommand(const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    if (InArgs.IsEmpty())
    {
        OutReturnCode = ECommandReturnCodes::MissingArgs;
        OutResponse   = TEXT("At least one argument is required.");
        return;
    }

    if (
        /*
         * Due to latency, we might have a rare case where one single client sends a message and disconnects right
         * after leaving an unpopulated dedicated server behind. Therefore, this iterator may be invalid.
         */
        FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator();
        It
    )
    {
        for (; It; ++It)
        {
            const APlayerController* PlayerController = It->Get();
            check( PlayerController )

            UChatComponent* Target = PlayerController->GetComponentByClass<UChatComponent>();
            Target->AddMessageToChatLog_ClientRPC(ChatStatics::AuthorityName, FText::FromString(FString::Join(InArgs, TEXT(" "))));

            continue;
        }
    }

    OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
    OutResponse  = L"";

    return;
}
