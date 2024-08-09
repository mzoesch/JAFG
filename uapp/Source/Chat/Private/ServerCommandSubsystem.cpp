// Copyright 2024 mzoesch. All rights reserved.

#include "ServerCommandSubsystem.h"

#include "ChatComponent.h"
#include "CommonNetworkStatics.h"
#include "JAFGLogDefs.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/WorldCharacter.h"

#define OWNING_SESSION                                                          \
    Cast<AWorldGameSession>(this->GetWorld()->GetAuthGameMode()->GameSession)
#define OWNER_CHARACTER                                                         \
    Cast<AWorldCharacter>(Cast<AController>(Owner->GetOwner())->GetCharacter())
#define DECLARE_OWNER_AS_CHAR_TARGET                                            \
    AWorldCharacter* Target =                                                   \
        Cast<AWorldCharacter>(                                                  \
            Cast<AController>(Owner->GetOwner()                                 \
        )->GetCharacter());
#define DECLARE_OWNER_AS_CTRL_TARGET                                            \
    AWorldPlayerController* Target =                                            \
        Cast<AWorldPlayerController>(Owner->GetOwner());
#define DECLARE_SERVER_COMMAND(Name, Callback)                                  \
    {                                                                           \
        const FServerCommand Command =                                          \
            FString::Printf(                                                    \
            TEXT("%s%s"),                                                       \
                *CommandStatics::ServerCommandPrefix, TEXT(Name)                \
            );                                                                  \
        this->ServerCommands.Add(Command,                                       \
            [this]                                                              \
            (                                                                   \
                UChatComponent* Owner,                                          \
                const TArray<FString>& InArgs,                                  \
                CommandReturnCode& OutReturnCode,                               \
                FString& OutResponse                                            \
            )                                                                   \
        {                                                                       \
            this->Callback(Owner, InArgs, OutReturnCode, OutResponse);          \
        });                                                                     \
    }

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
    DECLARE_SERVER_COMMAND( "help",      OnHelpCommand                        )
    DECLARE_SERVER_COMMAND( "broadcast", OnBroadcastCommand                   )
    DECLARE_SERVER_COMMAND( "fly",       OnFlyCommand                         )
    DECLARE_SERVER_COMMAND( "infly",     OnAllowInputFlyCommand               )
    DECLARE_SERVER_COMMAND( "players",   OnShowOnlinePlayersCommand           )
    DECLARE_SERVER_COMMAND( "kick" ,     OnKickCommand                        )
    DECLARE_SERVER_COMMAND( "name",      OnChangeDisplayNameCommand           )
    DECLARE_SERVER_COMMAND( "give",      OnGiveAccumulatedCommand             )
    DECLARE_SERVER_COMMAND( "showinv",   OnShowReadOnlyPlayerInventoryCommand )

    return;
}

UChatComponent* UServerCommandSubsystem::GetTargetBasedOnArgs(const TArray<FString>& Args, CommandReturnCode& OutReturnCode, const int32 Index /* = 0 */) const
{
    if (Args.IsValidIndex(Index) == false)
    {
        OutReturnCode = ECommandReturnCodes::MissingArgs;
        return nullptr;
    }

    OutReturnCode = ECommandReturnCodes::Success;

    const FString TargetName = Args[Index];
    const AWorldPlayerController* Target = OWNING_SESSION->GetPlayerControllerFromDisplayName(TargetName);

    return Target ? Target->GetComponentByClass<UChatComponent>() : nullptr;
}

void UServerCommandSubsystem::OnHelpCommand(SERVER_COMMAND_SIG) const
{
    for (const TPair<FServerCommand, FServerCommandCallback>& Pair : this->ServerCommands)
    {
        Owner->AddMessageToChatLog_ClientRPC(ChatStatics::AuthorityName, FText::FromString(Pair.Key));
    }

    OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
    OutResponse   = L"";

    return;
}

void UServerCommandSubsystem::OnBroadcastCommand(SERVER_COMMAND_SIG) const
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

// ReSharper disable once CppMemberFunctionMayBeStatic
void UServerCommandSubsystem::OnFlyCommand(SERVER_COMMAND_SIG) const
{
    DECLARE_OWNER_AS_CHAR_TARGET

    if (Target == nullptr)
    {
        LOG_WARNING(LogJAFGChat, "Target is invalid.")
        return;
    }

    Target->ToggleFly();

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = Target->IsFlying() ? TEXT("Flying enabled.") : TEXT("Flying disabled.");

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UServerCommandSubsystem::OnAllowInputFlyCommand(SERVER_COMMAND_SIG) const
{
    DECLARE_OWNER_AS_CHAR_TARGET

    if (Target == nullptr)
    {
        LOG_WARNING(LogJAFGChat, "Target is invalid.")
        return;
    }

    Target->ToggleInputFly();

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = Target->IsInputFlyEnabled() ? TEXT("Input fly enabled.") : TEXT("Input fly disabled.");

    return;
}

void UServerCommandSubsystem::OnShowOnlinePlayersCommand(SERVER_COMMAND_SIG) const
{
    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            const APlayerController* PlayerController = It->Get();
            check( PlayerController )

            OutResponse += FString::Printf(TEXT("%s, "), *PlayerController->GetComponentByClass<UChatComponent>()->GetPlayerDisplayName());

            continue;
        }

        Owner->AddMessageToChatLog_ClientRPC(ChatStatics::AuthorityName, FText::FromString(OutResponse));

        OutReturnCode = ECommandReturnCodes::SuccessNoResponse;
        OutResponse   = L"";

        return;
    }

    OutReturnCode = ECommandReturnCodes::Failure;
    OutResponse   = TEXT("No players online.");

    return;
}

void UServerCommandSubsystem::OnKickCommand(SERVER_COMMAND_SIG) const
{
    const UChatComponent* Target = this->GetTargetBasedOnArgs(InArgs, OutReturnCode);

    if (OutReturnCode == ECommandReturnCodes::MissingArgs)
    {
        OutResponse   = TEXT("At least one argument is required.");
        return;
    }

    if (Target == nullptr)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Target [%s] not found."), *InArgs[0]);
        return;
    }

    if (Target->GetPredictedOwner()->IsLocalController())
    {
        OutReturnCode = ECommandReturnCodes::Forbidden;
        OutResponse   = TEXT("Server operator cannot be kicked.");
        return;
    }

    /* Kick next tick. */
    AsyncTask(ENamedThreads::GameThread, [this, Target] (void)
    {
        OWNING_SESSION->KickPlayer(Target->GetPredictedOwner(), FText::FromString(TEXT("Kicked by operator.")));
    });

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = FString::Printf(TEXT("Kicked [%s]."), *Target->GetPlayerDisplayName());

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UServerCommandSubsystem::OnChangeDisplayNameCommand(SERVER_COMMAND_SIG) const
{
    if (InArgs.Num() != 1)
    {
        OutReturnCode = InArgs.Num() < 1 ? ECommandReturnCodes::MissingArgs : ECommandReturnCodes::TooManyArgs;
        OutResponse   = TEXT("One argument is required.");
        return;
    }

    DECLARE_OWNER_AS_CTRL_TARGET

    const FString OldName = Target->GetDisplayName();
    this->GetWorld()->GetAuthGameMode()->ChangeName(Target, InArgs[0], true);

    OutReturnCode = ECommandReturnCodes::SuccessBroadcastWithAuthority;
    OutResponse   = FString::Printf(TEXT("[%s] is now known as [%s]."), *OldName, *InArgs[0]);

    return;
}

void UServerCommandSubsystem::OnGiveAccumulatedCommand(SERVER_COMMAND_SIG) const
{
    const UChatComponent* Target = this->GetTargetBasedOnArgs(InArgs, OutReturnCode);

    if (OutReturnCode == ECommandReturnCodes::MissingArgs || InArgs.Num() < 2)
    {
        OutResponse   = TEXT("At least two arguments are required.");
        return;
    }

    if (Target == nullptr)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Target [%s] not found."), *InArgs[0]);
        return;
    }

    IContainer* TargetContainer = Cast<IContainer>(Target->GetPredictedOwner()->GetPawn());

    if (TargetContainer == nullptr)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Target [%s] is not a container."), *InArgs[0]);
        return;
    }

    FString AccumulatedName = InArgs[1];

    voxel_t AccumulatedIndex = this->GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetVoxelIndex("JAFG", AccumulatedName);

    if (AccumulatedIndex < ECommonVoxels::Num)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Accumulated [%s] not found or is not obtainable."), *AccumulatedName);
        return;
    }

    if (TargetContainer->EasyAddToContainer(FAccumulated(AccumulatedIndex)) == false)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Failed to add [%s] to [%s]."), *AccumulatedName, *InArgs[0]);
        return;
    }

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = FString::Printf(TEXT("Added [%s] to [%s]."), *AccumulatedName, *InArgs[0]);

    return;
}

void UServerCommandSubsystem::OnShowReadOnlyPlayerInventoryCommand(SERVER_COMMAND_SIG) const
{
    const UChatComponent* Target = this->GetTargetBasedOnArgs(InArgs, OutReturnCode);

    if (OutReturnCode == ECommandReturnCodes::MissingArgs || InArgs.Num() < 1)
    {
        OutResponse   = TEXT("At least one arguments are required.");
        return;
    }

    if (Target == nullptr)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Target [%s] not found."), *InArgs[0]);
        return;
    }

    IContainer* TargetContainer = Cast<IContainer>(Target->GetPredictedOwner()->GetPawn());

    if (TargetContainer == nullptr)
    {
        OutReturnCode = ECommandReturnCodes::Failure;
        OutResponse   = FString::Printf(TEXT("Target [%s] is not a container."), *InArgs[0]);
        return;
    }

    const FString InventoryString = TargetContainer->ToString_Container();

    OutReturnCode = ECommandReturnCodes::Success;
    OutResponse   = FString::Printf(TEXT("Inventory of [%s]: %s"), *InArgs[0], *InventoryString);

    return;
}

#undef OWNING_SESSION
#undef OWNER_CHARACTER
#undef DECLARE_OWNER_AS_CHAR_TARGET
#undef DECLARE_SERVER_COMMAND
