// Copyright 2024 mzoesch. All rights reserved.

#include "ChatComponent.h"

#include "ChatMenu.h"
#include "ClientCommandSubsystem.h"
#include "CommonChatStatics.h"
#include "Definitions.h"
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
        this->QueueMessage_ServerRPC(Message);
        return;
    }

    UClientCommandSubsystem* ClientCommandSubsystem = this->GetWorld()->GetSubsystem<UClientCommandSubsystem>();
    check( ClientCommandSubsystem )

    if (ClientCommandSubsystem->IsRegisteredClientCommand(Message) == false)
    {
        this->QueueMessage_ServerRPC(Message);
        return;
    }

    CommandReturnCode ReturnCode = 0;
    FString           Response   = L"";

    ClientCommandSubsystem->ExecuteCommand(Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCodes::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Client command subsystem returned invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCodes::Success)
    {
        this->AddMessageToChatLog(
            FString::Printf(TEXT("%s"), *ChatStatics::InternalName),
            FText::FromString(Response)
        );
        return;
    }

    if (ReturnCode == ECommandReturnCodes::SuccessNoResponse)
    {
        return;
    }

    if (ECommandReturnCodes::IsFailure(ReturnCode))
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to execute command [%s] with return code [%s].",
            *this->GetPlayerDisplayName(), *Message.ToString(), *ECommandReturnCodes::LexToString(ReturnCode)
        )
        this->AddMessageToChatLog(
            ChatStatics::InternalName,
            FText::FromString(FString::Printf(
                TEXT("Command [%s] failed with return code [%s]."),
                *Message.ToString(), *ECommandReturnCodes::LexToString(ReturnCode)
            ))
        );
        return;
    }

    checkNoEntry()

    return;
}

FString UChatComponent::GetPlayerDisplayName(void) const
{
    return this->GetPredictedOwner()->GetDisplayName();
}

bool UChatComponent::QueueMessage_ServerRPC_Validate(const FText& Message)
{
    return ChatStatics::IsTextValid(Message);
}

void UChatComponent::QueueMessage_ServerRPC_Implementation(const FText& Message)
{
    if (CommandStatics::IsCommand(Message) == false)
    {
        this->BroadcastMessage(Message);
        return;
    }

    const UServerCommandSubsystem* ServerCommandSubsystem = this->GetWorld()->GetSubsystem<UServerCommandSubsystem>();
    check( ServerCommandSubsystem )

    CommandReturnCode ReturnCode = 0;
    FString           Response  = L"";

    ServerCommandSubsystem->ExecuteCommand(this, Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCodes::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Server command subsystem returned an invalid return code.")
        return;
    }

    if (ReturnCode == ECommandReturnCodes::Success)
    {
        this->AddMessageToChatLog_ClientRPC(
            FString::Printf(TEXT("%s"), *ChatStatics::AuthorityName),
            FText::FromString(Response)
        );
        return;
    }

    if (ReturnCode == ECommandReturnCodes::SuccessNoResponse)
    {
        return;
    }

    if (ECommandReturnCodes::IsFailure(ReturnCode))
    {
        LOG_WARNING(
            LogJAFGChat,
            "Player [%s] attempted to execute command [%s] with return code [%s]. Response [%s].",
            *this->GetPlayerDisplayName(), *Message.ToString(), *ECommandReturnCodes::LexToString(ReturnCode), *Response
        )
        /*
         * We want to give the client as little information as possible about the failure.
         * If they are an administrator, they may want to check the logs then.
         */
        this->AddMessageToChatLog_ClientRPC(
            ChatStatics::AuthorityName,
            FText::FromString(TEXT("Unknown command or command can not executated by you."))
        );
        return;
    }

    checkNoEntry()

    return;
}

void UChatComponent::BroadcastMessage(const FText& Message) const
{
    LOG_VERBOSE(LogJAFGChat, "Queueing message [%s]. Pending clients: %d.", *Message.ToString(), this->GetWorld()->GetNumPlayerControllers())

    const FString Sender = this->GetPlayerDisplayName();

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
            Target->AddMessageToChatLog_ClientRPC(Sender, Message);

            continue;
        }
    }

    return;
}

void UChatComponent::AddMessageToChatLog(const FString& Sender, const FText& Message)
{
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this->GetWorld(), Widgets, UChatMenu::StaticClass());

    UChatMenu* ChatMenu = Widgets.Num() > 0 ? Cast<UChatMenu>(Widgets[0]) : nullptr;

    if (ChatMenu == nullptr)
    {
        this->PreChatWidgetConstructionQueue.Enqueue(FPrivateMessagePreConstruct{Sender, Message});
        return;
    }

    ChatMenu->AddMessageToChatLog(Sender, Message);
}

void UChatComponent::AddMessageToChatLog_ClientRPC_Implementation(const FString& Sender, const FText& Message)
{
    this->AddMessageToChatLog(Sender, Message);
}
