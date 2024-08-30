// Copyright 2024 mzoesch. All rights reserved.

#include "Commands/JAFGCoreChatCommands.h"
#include "ChatComponentImpl.h"
#include "ChatMessage.h"
#include "ChatStatics.h"
#include "LocalSessionSupervisorSubsystem.h"
#include "Chat/ChatComponent.h"
#include "Commands/ShippedWorldChatCommands.h"
#include "WorldCore/WorldCharacter.h"
#include "Player/WorldPlayerController.h"

#define DECLARE_CALLEE_AS_CHAR_TARGET() \
    AWorldCharacter* Target = GetWorldCharacter(Params.Callee); \
    if (Target == nullptr) \
    { \
        Params.OutReturnCode = ECommandReturnCode::Forbidden; \
        return; \
    }
#define DECLARE_CALLEE_AS_CTRL_TARGET() \
    AWorldPlayerController* Target = GetWorldController(Params.Callee); \
    if (Target == nullptr) \
    { \
        Params.OutReturnCode = ECommandReturnCode::Forbidden; \
        return; \
    }

FORCEINLINE auto GetWorldCharacter(const UChatComponent* Component) -> AWorldCharacter*
{
    return Cast<AWorldCharacter>(Cast<AController>(Component->GetOwner())->GetCharacter());
}

FORCEINLINE auto GetWorldController(const UChatComponent* Component) -> AWorldPlayerController*
{
    return Cast<AWorldPlayerController>(Component->GetOwner());
}

void ChatCommands::RegisterJAFGCoreChatCommands(UShippedWorldChatCommandRegistry* ChatCommands)
{
    jcheck( ChatCommands )

    using namespace ::ChatCommands;
    using namespace ::ChatCommands::Callbacks;
    using namespace ::ChatCommands::Callbacks::Client;
    using namespace ::ChatCommands::Callbacks::Authority;

    {
        const FChatCommand Command = FString::Printf(
            TEXT("%s%s"),
            *CommandStatics::ClientCommandPrefix, TEXT("help")
        );
        FChatCommandObject CommandObject;
        CommandObject.Command  = Command;
        CommandObject.Callback = OnHelp;

        ChatCommands->RegisterCommand(CommandObject);
    }

    {
        const FChatCommand Command = FString::Printf(
            TEXT("%s%s"),
            *CommandStatics::ClientCommandPrefix, TEXT("quit")
        );
        FChatCommandObject CommandObject;
        CommandObject.Command  = Command;
        CommandObject.Callback = OnQuit;

        ChatCommands->RegisterCommand(CommandObject);
    }

    {
        const FChatCommand Command = FString::Printf(
            TEXT("%s%s"),
            *CommandStatics::ClientCommandPrefix, TEXT("clear")
        );
        FChatCommandObject CommandObject;
        CommandObject.Command  = Command;
        CommandObject.Callback = OnClear;

        ChatCommands->RegisterCommand(CommandObject);
    }

    if (UNetStatics::IsSafeServer(ChatCommands) == false)
    {
        return;
    }

    {
        const FChatCommand Command = FString::Printf(
            TEXT("%s%s"),
            *CommandStatics::ServerCommandPrefix, TEXT("broadcast")
        );
        FChatCommandObject CommandObject;
        CommandObject.Command  = Command;
        CommandObject.Callback = OnBroadcast;
        CommandObject.Syntax.Emplace(EChatCommandSyntax::SharpSharpAny);

        ChatCommands->RegisterCommand(CommandObject);
    }

    {
        const FChatCommand Command = FString::Printf(
            TEXT("%s%s"),
            *CommandStatics::ServerCommandPrefix, TEXT("give")
        );
        FChatCommandObject CommandObject;
        CommandObject.Command  = Command;
        CommandObject.Callback = OnGiveAccumulated;
        CommandObject.Syntax.Emplace(EChatCommandSyntax::PlayerName);
        CommandObject.Syntax.Emplace(EChatCommandSyntax::Accumulated);
        CommandObject.Syntax.Emplace(EChatCommandSyntax::AccAmount);

        ChatCommands->RegisterCommand(CommandObject);
    }

    return;
}

void ChatCommands::Callbacks::Client::OnHelp(const FChatCommandParams& Params)
{
    struct FChatLogEntry final { FString Command; FString Info; };

    TArray<FChatLogEntry> LogEntries;

    for (const TTuple<FChatCommand, FChatCommandObject>& Obj : Params.Callee->GetShippedWorldChatCommands()->GetRegisteredCommands())
    {
        LogEntries.Emplace(FChatLogEntry(Obj.Key, Obj.Value.Info));
    }
    for (const TTuple<FChatCommand, FChatCommandObject>& Obj : Params.Callee->GetShippedWorldChatCommands()->GetRegisteredRemoteCommands())
    {
        LogEntries.Emplace(FChatLogEntry(Obj.Key, Obj.Value.Info));
    }

    check( LogEntries.IsEmpty() == false )

    LogEntries.Sort( [] (const FChatLogEntry& A, const FChatLogEntry& B) -> bool
    {
        return A.Command < B.Command;
    });

    for (const auto& [Command, Info] : LogEntries)
    {
        Params.Callee->AddMessageToChatLog(FChatMessage(FString::Printf(
            TEXT("%s - %s"),
            *Command,
            Info.IsEmpty() ? TEXT("NO INFO") : *Info
        )));
    }

    Params.OutReturnCode = ECommandReturnCode::SuccessNoResponse;

    return;
}

void ChatCommands::Callbacks::Client::OnClear(const FChatCommandParams& Params)
{
    Params.Callee->ClearAllMessagesFromChatLog();

    Params.OutReturnCode = ECommandReturnCode::SuccessNoResponse;

    return;
}

void ChatCommands::Callbacks::Client::OnQuit(const FChatCommandParams& Params)
{
    ULocalSessionSupervisorSubsystem* LSSSS = Params.Callee->GetWorld()->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
    LSSSS->LeaveSession();

    Params.OutReturnCode = ECommandReturnCode::SuccessNoResponse;

    return;
}

void ChatCommands::Callbacks::Authority::OnBroadcast(const FChatCommandParams& Params)
{
    if (Params.InArgs.IsEmpty())
    {
        Params.OutReturnCode = ECommandReturnCode::MissingArgs;
        Params.OutResponse   = TEXT("Missing message to broadcast.");
        return;
    }

    if (
        /*
         * Due to latency, we might have a rare case where one single client sends a message and disconnects right
         * after leaving an unpopulated dedicated server behind. Therefore, this iterator may be invalid.
         */
        FConstPlayerControllerIterator It = Params.Callee->GetWorld()->GetPlayerControllerIterator();
        It
    )
    {
        for (; It; ++It)
        {
            const APlayerController* PlayerController = It->Get();
            if (PlayerController == nullptr)
            {
                continue;
            }

            UChatComponentImpl* Target = PlayerController->GetComponentByClass<UChatComponentImpl>();
            check( Target )
            Target->AddMessageToChatLog(FChatMessage(
                EChatMessageType::Authority,
                ChatStatics::AuthorityName,
                FText::FromString(FString::Join(Params.InArgs, TEXT(" ")))
            ));

            continue;
        }
    }

    Params.OutReturnCode = ECommandReturnCode::SuccessNoResponse;

    return;
}

void ChatCommands::Callbacks::Authority::OnFly(const FChatCommandParams& Params)
{
    DECLARE_CALLEE_AS_CHAR_TARGET()

    Target->ToggleFly();

    Params.OutReturnCode = ECommandReturnCode::Success;
    Params.OutResponse   = Target->IsFlying() ? TEXT("Flying enabled.") : TEXT("Flying disabled.");

    return;
}

void ChatCommands::Callbacks::Authority::OnAllowInputFly(const FChatCommandParams& Params)
{
    DECLARE_CALLEE_AS_CHAR_TARGET()

    Target->ToggleInputFly();

    Params.OutReturnCode = ECommandReturnCode::Success;
    Params.OutResponse   = Target->IsFlying() ? TEXT("Input fly enabled.") : TEXT("Input fly disabled.");

    return;
}

void ChatCommands::Callbacks::Authority::OnShowOnlinePlayers(const FChatCommandParams& Params)
{
    if (FConstPlayerControllerIterator It = Params.Callee->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            const APlayerController* PlayerController = It->Get();

            if (PlayerController == nullptr)
            {
                continue;
            }

            Params.OutResponse  += FString::Printf(TEXT("%s, "), *PlayerController->GetComponentByClass<UChatComponentImpl>()->GetPlayerDisplayName());

            continue;
        }

        Params.OutReturnCode = ECommandReturnCode::Success;

        return;
    }

    Params.OutReturnCode = ECommandReturnCode::Success;
    Params.OutResponse   = TEXT("No players online.");

    return;
}

void ChatCommands::Callbacks::Authority::OnKick(const FChatCommandParams& Params)
{
}

void ChatCommands::Callbacks::Authority::OnChangeDisplayName(const FChatCommandParams& Params)
{
    DECLARE_CALLEE_AS_CTRL_TARGET()

    if (Params.InArgs.Num() != 0)
    {
        Params.OutReturnCode = Params.InArgs.Num() > 1 ? ECommandReturnCode::TooManyArgs : ECommandReturnCode::MissingArgs;

        return;
    }

    const FString OldName = Target->GetDisplayName();

    if (OldName == Params.InArgs[0])
    {
        Params.OutReturnCode = ECommandReturnCode::SemanticError;
        Params.OutResponse   = TEXT("A name has to be different.");
        return;
    }

    Params.Callee->GetWorld()->GetAuthGameMode()->ChangeName(Target, Params.InArgs[0], true);

    Params.OutReturnCode = ECommandReturnCode::SuccessBroadcastWithAuthority;
    Params.OutResponse   = FString::Printf(TEXT("[%s] is now known as [%s]."), *OldName, *Params.InArgs[0]);

    return;
}

void ChatCommands::Callbacks::Authority::OnGiveAccumulated(const FChatCommandParams& Params)
{
    Params.OutReturnCode = ECommandReturnCode::Success;
}

void ChatCommands::Callbacks::Authority::OnShowReadOnlyPlayerInventory(const FChatCommandParams& Params)
{
}

#undef DECLARE_CALLEE_AS_CHAR_TARGET
#undef DECLARE_CALLEE_AS_CTRL_TARGET
