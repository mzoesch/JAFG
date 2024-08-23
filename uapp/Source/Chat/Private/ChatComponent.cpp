// Copyright 2024 mzoesch. All rights reserved.

#include "ChatComponent.h"
#include "ChatMenu.h"
#include "ClientCommandSubsystem.h"
#include "CommonChatStatics.h"
#include "JAFGLogDefs.h"
#include "ServerCommandSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Player/WorldPlayerController.h"

UChatComponent::UChatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
    this->ChatStdInLog.Empty();
    return;
}

void UChatComponent::BeginPlay(void)
{
    Super::BeginPlay();

    this->ChatStdInLog.Empty();

    return;
}

AWorldPlayerController* UChatComponent::GetPredictedOwner(void) const
{
    return Cast<AWorldPlayerController>(this->GetOwner());
}

UChatMenu* UChatComponent::GetChatMenu(void) const
{
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this->GetWorld(), Widgets, UChatMenu::StaticClass());
    return Widgets.Num() > 0 ? Cast<UChatMenu>(Widgets[0]) : nullptr;
}

auto UChatComponent::GetSafeChatMenu(void) const -> UChatMenu*
{
    if (UChatMenu* ChatMenu = this->GetChatMenu(); ChatMenu != nullptr)
    {
        return ChatMenu;
    }

    LOG_FATAL(LogJAFGChat, "Chat menu widget is not initialized.")

    return nullptr;
}

void UChatComponent::ParseMessage(const FText& Message)
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Parsing message: %s", *Message.ToString())

    if (this->ChatStdInLog.IsEmpty())
    {
        this->ChatStdInLog.EmplaceAt(0, Message);
    }
    if (this->ChatStdInLog[0].EqualTo(Message) == false)
    {
        this->ChatStdInLog.EmplaceAt(0, Message);
    }
    while (this->ChatStdInLog.Num() > UChatComponent::MaxChatStdInLogBuffered)
    {
        this->ChatStdInLog.Pop();
    }

    if (CommandStatics::IsCommand(Message) == false)
    {
        this->QueueMessage_ServerRPC(FChatMessage(
            EChatMessageType::Player,
            FString::Printf(TEXT("%s"), *this->GetPlayerDisplayName()),
            Message
        ));
        return;
    }

    const UClientCommandSubsystem* ClientCommandSubsystem = this->GetWorld()->GetSubsystem<UClientCommandSubsystem>();
    check( ClientCommandSubsystem )

    if (ClientCommandSubsystem->IsRegisteredClientCommand(Message) == false)
    {
        this->QueueMessage_ServerRPC(FChatMessage(
            EChatMessageType::Player,
            FString::Printf(TEXT("%s"), *this->GetPlayerDisplayName()),
            Message
        ));
        return;
    }

    CommandReturnCode ReturnCode = 0;
    FString           Response   = TEXT("");

    ClientCommandSubsystem->ExecuteCommand(Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCode::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Client command subsystem returned invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCode::Success)
    {
        this->AddMessageToChatLog(FChatMessage(
            EChatMessageType::Client,
            FString::Printf(TEXT("%s"), *ChatStatics::InternalName),
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
            *this->GetPlayerDisplayName(), *Message.ToString(), *ECommandReturnCode::LexToString(ReturnCode)
        )
        this->AddMessageToChatLog(FChatMessage(
            EChatMessageType::Client,
            FString::Printf(TEXT("%s"), *ChatStatics::InternalName),
            FText::FromString(FString::Printf(
                TEXT("Command [%s] failed with return code [%s]."),
                *Message.ToString(), *ECommandReturnCode::LexToString(ReturnCode)
            ))
        ));
        return;
    }

    checkNoEntry()

    return;
}

FString UChatComponent::GetPlayerDisplayName(void) const
{
    return this->GetPredictedOwner()->GetDisplayName();
}

bool UChatComponent::QueueMessage_ServerRPC_Validate(const FChatMessage& Message)
{
    if (ChatStatics::IsTextValid(Message.Message) == false)
    {
        LOG_WARNING(LogJAFGChat, "Player [%s] attempted to send an invalid message. Message: [].", *this->GetPlayerDisplayName(), *Message.Message.ToString())
#if WITH_STRIKE_SUBSYSTEM
    #error "Strike subsystem is not implemented here."
#endif /* WITH_STRIKE_SUBSYSTEM */
        return false;
    }

    return true;
}

void UChatComponent::QueueMessage_ServerRPC_Implementation(const FChatMessage& Message)
{
    if (CommandStatics::IsCommand(Message.Message) == false)
    {
        this->BroadcastMessage(Message);
        return;
    }

    const UServerCommandSubsystem* ServerCommandSubsystem = this->GetWorld()->GetSubsystem<UServerCommandSubsystem>();
    check( ServerCommandSubsystem )

    CommandReturnCode ReturnCode = 0;
    FString           Response  = L"";

    ServerCommandSubsystem->ExecuteCommand(this, Message.Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCode::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Server command subsystem returned an invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCode::Success)
    {
        this->AddMessageToChatLog_ClientRPC(FChatMessage(
            EChatMessageType::Authority,
            FString::Printf(TEXT("%s"), *ChatStatics::AuthorityName),
            FText::FromString(Response)
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
            ReturnCode == ECommandReturnCode::SuccessBroadcastWithAuthority ? EChatMessageType::Authority : EChatMessageType::Authority,
            FString::Printf(TEXT("%s"), *ChatStatics::AuthorityName),
            FText::FromString(Response)
        ));

        return;
    }

    if (ECommandReturnCode::IsFailure(ReturnCode))
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to execute command [%s] with return code [%s]. Response [%s].",
            *this->GetPlayerDisplayName(), *Message.Message.ToString(), *ECommandReturnCode::LexToString(ReturnCode), *Response
        )
        if (this->GetPredictedOwner()->IsLocalController())
        {
            this->AddMessageToChatLog_ClientRPC(FChatMessage(
                EChatMessageType::Authority,
                FString::Printf(TEXT("%s"), *ChatStatics::AuthorityName),
                FText::FromString(FString::Printf(
                    TEXT("Command [%s] failed with return code [%s]. Response [%s]."),
                    *Message.Message.ToString(), *ECommandReturnCode::LexToString(ReturnCode), *Response
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
            FString::Printf(TEXT("%s"), *ChatStatics::AuthorityName),
            FText::FromString(TEXT("Unknown command or command can not executed by you."))
        ));
        return;
    }

    checkNoEntry()

    return;
}

void UChatComponent::BroadcastMessage(const FChatMessage& Message) const
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

            UChatComponent* Target = PlayerController->GetComponentByClass<UChatComponent>();
            Target->AddMessageToChatLog_ClientRPC(Message);

            continue;
        }
    }

    return;
}

void UChatComponent::AddMessageToChatLog(const FChatMessage& Message)
{
    UChatMenu* ChatMenu = this->GetChatMenu();

    if (ChatMenu == nullptr)
    {
        this->PreChatWidgetConstructionQueue.Enqueue(Message);
        return;
    }

    ChatMenu->AddMessageToChatLog(Message);
}

void UChatComponent::AddMessageToChatLog_ClientRPC_Implementation(const FChatMessage& Message)
{
    this->AddMessageToChatLog(Message);
}
