// Copyright 2024 mzoesch. All rights reserved.

#include "ServerCommandSubsystem.h"

#include "ChatComponent.h"
#include "CommonNetworkStatics.h"
#include "Definitions.h"
#include "WorldCore/WorldCharacter.h"

#define OWNER_CHARACTER Cast<AWorldCharacter>(Cast<AController>(Owner->GetOwner())->GetCharacter())
#define DECLARE_OWNER_AS_TARGET AWorldCharacter* Target = OWNER_CHARACTER;

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

void UServerCommandSubsystem::ExecuteCommand(UChatComponent* Owner, const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    TArray<FString> Args;
    FServerCommand Command = CommandStatics::GetCommandWithArgs(StdIn, Args);
    Command = CommandStatics::SafePrefixServerCommand(Command);
    this->ExecuteCommand(Owner,Command, Args, OutReturnCode, OutResponse);

    return;
}

void UServerCommandSubsystem::ExecuteCommand(UChatComponent* Owner, const FServerCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Executing server command: [%s].", *Command)

    if (this->IsRegisteredCommand(Command) == false)
    {
        OutReturnCode = ECommandReturnCodes::Unknown;
        OutResponse   = L"";

        return;
    }

    this->ServerCommands[Command](Owner, Args, OutReturnCode, OutResponse);

    return;
}

void UServerCommandSubsystem::InitializeAllCommands(void)
{
    // Help
    //////////////////////////////////////////////////////////////////////////
    {
        const FServerCommand Command = TEXT("sv_help");
        this->ServerCommands.Add(Command, [this] (UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        {
            this->OnHelpCommand(Owner, InArgs, OutReturnCode, OutResponse);
        });
    }

    // Broadcast
    //////////////////////////////////////////////////////////////////////////
    {
        const FServerCommand Command = TEXT("sv_broadcast");
        this->ServerCommands.Add(Command, [this] (UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        {
            this->OnBroadcastCommand(Owner, InArgs, OutReturnCode, OutResponse);
        });
    }

    // Fly
    //////////////////////////////////////////////////////////////////////////
    {
        const FServerCommand Command = TEXT("sv_fly");
        this->ServerCommands.Add(Command, [this] (UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        {
            this->OnFlyCommand(Owner, InArgs, OutReturnCode, OutResponse);
        });
    }

    // Allow Input Fly
    //////////////////////////////////////////////////////////////////////////
    {
        const FServerCommand Command = TEXT("sv_infly");
        this->ServerCommands.Add(Command, [this] (UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse)
        {
            this->OnAllowInputFlyCommand(Owner, InArgs, OutReturnCode, OutResponse);
        });
    }
}

void UServerCommandSubsystem::OnHelpCommand(UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    for (const TPair<FServerCommand, FServerCommandCallback>& Pair : this->ServerCommands)
    {
        Owner->AddMessageToChatLog_ClientRPC(ChatStatics::AuthorityName, FText::FromString(Pair.Key));
    }

    OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
    OutResponse   = L"";

    return;
}

void UServerCommandSubsystem::OnBroadcastCommand(UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
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

void UServerCommandSubsystem::OnFlyCommand(UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    DECLARE_OWNER_AS_TARGET

    Target->ToggleFly();

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = Target->IsFlying() ? TEXT("Flying enabled.") : TEXT("Flying disabled.");

    return;
}

void UServerCommandSubsystem::OnAllowInputFlyCommand(UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const
{
    DECLARE_OWNER_AS_TARGET

    Target->ToggleInputFly();

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = Target->IsInputFlyEnabled() ? TEXT("Input fly enabled.") : TEXT("Input fly disabled.");


    return;
}

#undef OWNER_CHARACTER
#undef DECLARE_OWNER_AS_TARGET
