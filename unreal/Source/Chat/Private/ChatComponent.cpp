// Copyright 2024 mzoesch. All rights reserved.

#include "ChatComponent.h"

#include "ChatMenu.h"
#include "Definitions.h"
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
    this->QueueMessage_ServerRPC(Message);
    return;
}

bool UChatComponent::QueueMessage_ServerRPC_Validate(const FText& Message)
{
    return ChatStatics::IsTextValid(Message);
}

void UChatComponent::QueueMessage_ServerRPC_Implementation(const FText& Message)
{
    LOG_VERBOSE(LogJAFGChat, "Queueing message [%s]. Pending clients: %d.", *Message.ToString(), this->GetWorld()->GetNumPlayerControllers())

    const FString Sender = this->GetOwner()->GetHumanReadableName();

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
