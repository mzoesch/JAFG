// Copyright 2024 mzoesch. All rights reserved.

#include "ChatComponentImpl.h"
#include "ChatMenu.h"
#include "ChatStatics.h"
#include "JAFGLogDefs.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Commands/ChatCommandStatics.h"
#include "Commands/ShippedWorldChatCommands.h"
#include "Player/WorldPlayerController.h"

UChatComponentImpl::UChatComponentImpl(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
    this->ChatStdInLog.Empty();
    return;
}

void UChatComponentImpl::BeginPlay(void)
{
    Super::BeginPlay();

    this->ChatStdInLog.Empty();

    return;
}

UShippedWorldChatCommandRegistry* UChatComponentImpl::GetShippedWorldChatCommands(void) const
{
    return this->GetWorld()->GetSubsystem<UShippedWorldChatCommandRegistry>();
}

void UChatComponentImpl::AddMessageToChatLog(const FChatMessage& Message)
{
    if (this->GetPredictedOwner()->IsLocalController())
    {
        if (UChatMenu* ChatMenu = this->GetChatMenu(); ChatMenu)
        {
            ChatMenu->AddMessageToChatLog(Message);
        }
        else
        {
            this->PreChatWidgetConstructionQueue.Enqueue(Message);
        }
    }
    else
    {
        this->AddMessageToChatLog_ClientRPC(Message);
    }

    return;
}

void UChatComponentImpl::ClearAllMessagesFromChatLog(void)
{
    this->GetSafeChatMenu()->ClearAllChatEntries();
}

void UChatComponentImpl::QueueCommandForServer(const FText& StdIn)
{
    if (UNetStatics::IsSafeServer(this))
    {
        LOG_FATAL(LogJAFGChat, "Server attempted to queue a remote procedure call to itself.")
        return;
    }

    this->QueueCommand_ServerRPC(StdIn);

    return;
}

AWorldPlayerController* UChatComponentImpl::GetPredictedOwner(void) const
{
    return Cast<AWorldPlayerController>(this->GetOwner());
}

FString UChatComponentImpl::GetPlayerDisplayName(void) const
{
    return this->GetPredictedOwner()->GetDisplayName();
}

UChatMenu* UChatComponentImpl::GetChatMenu(void) const
{
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this->GetWorld(), Widgets, UChatMenu::StaticClass());
    return Widgets.Num() > 0 ? Cast<UChatMenu>(Widgets[0]) : nullptr;
}

auto UChatComponentImpl::GetSafeChatMenu(void) const -> UChatMenu*
{
    if (UChatMenu* ChatMenu = this->GetChatMenu(); ChatMenu != nullptr)
    {
        return ChatMenu;
    }

    LOG_FATAL(LogJAFGChat, "Chat menu widget is not initialized.")

    return nullptr;
}

void UChatComponentImpl::ParseMessage(const FText& Message)
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Parsing message: [%s]", *Message.ToString())

    if (this->ChatStdInLog.IsEmpty())
    {
        this->ChatStdInLog.EmplaceAt(0, Message);
    }
    if (this->ChatStdInLog[0].EqualTo(Message) == false)
    {
        this->ChatStdInLog.EmplaceAt(0, Message);
    }
    while (this->ChatStdInLog.Num() > UChatComponentImpl::MaxChatStdInLogBuffered)
    {
        this->ChatStdInLog.Pop();
    }

    if (CommandStatics::IsCommand(Message) == false)
    {
        this->QueueMessage_ServerRPC(Message);
        return;
    }

    const UShippedWorldChatCommandRegistry* ShippedWorldChatCommands = this->GetShippedWorldChatCommands();
    check( ShippedWorldChatCommands )

    CommandReturnCode ReturnCode = 0;
    FString           Response   = TEXT("");
    ShippedWorldChatCommands->ExecuteCommand(Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCode::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Client command subsystem re turned invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCode::Success)
    {
        this->AddMessageToChatLog(FChatMessage(
            EChatMessageType::Client,
            EChatMessageFormat::Success,
            FText::FromString(Response)
        ));
        return;
    }

    if (ReturnCode == ECommandReturnCode::SuccessNoResponse)
    {
        return;
    }

    if (ECommandReturnCode::IsFailure(ReturnCode))
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to execute command [%s] with return code [%s].",
            *this->GetPlayerDisplayName(), *Message.ToString(), *LexToString(ReturnCode)
        )
        this->AddMessageToChatLog(FChatMessage(
            EChatMessageType::Client,
            EChatMessageFormat::Error,
            FText::FromString(FString::Printf(
                TEXT("Command [%s] failed with return code [%s]."),
                *Message.ToString(), *LexToString(ReturnCode)
            ))
        ));
        return;
    }

    checkNoEntry()

    return;
}

bool UChatComponentImpl::QueueMessage_ServerRPC_Validate(const FText& Message)
{
    if (ChatStatics::IsTextValid(Message) == false)
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to send an invalid message. Message: [%s].",
            *this->GetPlayerDisplayName(),
            *Message.ToString()
        )
#if WITH_STRIKE_SUBSYSTEM
    #error "Strike subsystem is not implemented here."
#endif /* WITH_STRIKE_SUBSYSTEM */
        return false;
    }

    return true;
}

void UChatComponentImpl::QueueMessage_ServerRPC_Implementation(const FText& Message)
{
    if (CommandStatics::IsCommand(Message) == false)
    {
        this->BroadcastMessage(FChatMessage(this->GetPlayerDisplayName(), Message));
        return;
    }

#if WITH_STRIKE_SUBSYSTEM
    #error "Strike subsystem is not implemented here."
#endif /* WITH_STRIKE_SUBSYSTEM */

    return;
}

bool UChatComponentImpl::QueueCommand_ServerRPC_Validate(const FText& Command)
{
    const UShippedWorldChatCommandRegistry* ShippedWorldChatCommands = this->GetShippedWorldChatCommands();
    check( ShippedWorldChatCommands )

    return ShippedWorldChatCommands->IsCommandRegistered(CommandStatics::GetCommand(Command));
}

void UChatComponentImpl::QueueCommand_ServerRPC_Implementation(const FText& Command)
{
    const UShippedWorldChatCommandRegistry* ShippedWorldChatCommands = this->GetShippedWorldChatCommands();
    check( ShippedWorldChatCommands )

    CommandReturnCode ReturnCode = 0;
    FString           Response   = TEXT("");

    ShippedWorldChatCommands->ExecuteCommand(this, Command, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCode::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Server command subsystem returned an invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCode::Success)
    {
        this->AddMessageToChatLog_ClientRPC(FChatMessage(
            EChatMessageType::Authority,
            EChatMessageFormat::Success,
            ChatStatics::AuthorityName,
            Response
        ));

        return;
    }

    if (ReturnCode == ECommandReturnCode::SuccessNoResponse)
    {
        return;
    }

    if (ReturnCode == ECommandReturnCode::SuccessBroadcast || ReturnCode == ECommandReturnCode::SuccessBroadcastWithAuthority)
    {
        if (Response.IsEmpty())
        {
            LOG_FATAL(LogJAFGChat, "Server command subsystem returned success broadcast without a response.")
            return;
        }

        this->BroadcastMessage(FChatMessage(
            ReturnCode == ECommandReturnCode::SuccessBroadcastWithAuthority ? EChatMessageType::Authority : EChatMessageType::Player,
            ReturnCode == ECommandReturnCode::SuccessBroadcastWithAuthority ? ChatStatics::AuthorityName : this->GetPlayerDisplayName(),
            Response
        ));

        return;
    }

    if (ECommandReturnCode::IsFailure(ReturnCode))
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to execute command [%s] with return code [%s].%s",
            *this->GetPlayerDisplayName(), *Command.ToString(), *LexToString(ReturnCode),
            Response.IsEmpty() ? TEXT(" No response.") : *FString::Printf(TEXT(" Response: [%s]."), *Response)
        )

        if (this->GetPredictedOwner()->IsLocalController())
        {
            this->AddMessageToChatLog(FChatMessage(
                EChatMessageType::Authority,
                EChatMessageFormat::Error,
                ChatStatics::AuthorityName,
                FText::FromString(FString::Printf(
                    TEXT("Command [%s] failed with return code [%s].%s"),
                    *Command.ToString(), *LexToString(ReturnCode),
                    Response.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(" Response: [%s]."), *Response)
                ))
            ));

            return;
        }

        /*
         * We want to give the client as little information as possible about the failure.
         * If they are an administrator, they may want to check the logs then.
         */
        this->AddMessageToChatLog_ClientRPC(FChatMessage(
            EChatMessageType::Authority,
            EChatMessageFormat::Error,
            FString::Printf(TEXT("[%s] was rejected by authority."), *CommandStatics::GetCommand(Command))
        ));

        return;
    }

    checkNoEntry()

    return;
}

void UChatComponentImpl::BroadcastMessage(const FChatMessage& Message) const
{
    LOG_VERBOSE(LogJAFGChat, "Queueing message [%s]. Pending clients: %d.", *Message.Message.ToString(), this->GetWorld()->GetNumPlayerControllers())

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

            UChatComponentImpl* Target = PlayerController->GetComponentByClass<UChatComponentImpl>();
            Target->AddMessageToChatLog_ClientRPC(Message);

            continue;
        }
    }

    return;
}

void UChatComponentImpl::AddMessageToChatLog_ClientRPC_Implementation(const FChatMessage& Message)
{
    this->AddMessageToChatLog(Message);
}
