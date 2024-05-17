// Copyright 2024 mzoesch. All rights reserved.

#include "ChatComponent.h"

#include "ChatMenu.h"
#include "ClientCommandSubsystem.h"
#include "CommonChatStatics.h"
#include "Definitions.h"
#include "ServerCommandSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UChatComponent::UChatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
}

void UChatComponent::BeginPlay(void)
{
    Super::BeginPlay();
}

void UChatComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UChatComponent::ParseMessage(const FText& Message)
{
    LOG_VERY_VERBOSE(LogJAFGChat, "Parsing message: %s", *Message.ToString())

    if (CommandStatics::IsCommand(Message))
    {
        if (
            UClientCommandSubsystem* ClientCommandSubsystem = this->GetWorld()->GetSubsystem<UClientCommandSubsystem>();
            ClientCommandSubsystem->IsRegisteredClientCommand(Message)
        )
        {
            int32   ErrorCode = 0;
            FString Response  = L"";
            ClientCommandSubsystem->ExecuteCommand(Message, ErrorCode, Response);
            return;
        }
    }

    this->QueueMessage_ServerRPC(Message);

    return;
}

FString UChatComponent::GetPlayerDisplayName(void) const
{
    return this->GetOwner()->GetHumanReadableName();
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

    ServerCommandSubsystem->ExecuteCommand(Message, ReturnCode, Response);

    if (ReturnCode == ECommandReturnCodes::Invalid)
    {
        LOG_FATAL(LogJAFGChat, "Server command subsystem returned invalid return code.")
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
            "Player [%s] attempted to execute command [%s] with return code [%s].",
            *this->GetPlayerDisplayName(), *Message.ToString(), *ECommandReturnCodes::LexToString(ReturnCode)
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

void UChatComponent::AddMessageToChatLog_ClientRPC_Implementation(const FString& Sender, const FText& Message)
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

    return;
}
