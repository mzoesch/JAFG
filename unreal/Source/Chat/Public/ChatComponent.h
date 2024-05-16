// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ChatComponent.generated.h"

namespace ChatStatics
{

extern inline constexpr int32 MaxChatInputLength = 0x3F;

FORCEINLINE bool IsTextToLong(const FText& Text)
{
    return Text.ToString().Len() > MaxChatInputLength;
}

FORCEINLINE bool IsTextValid(const FText& Text)
{
    return Text.ToString().Len() > 0 && ChatStatics::IsTextToLong(Text) == false;
}

}

struct FPrivateMessagePreConstruct final
{
    FString Sender;
    FText   Message;
};

UCLASS(NotBlueprintable)
class CHAT_API UChatComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UChatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /**
     * Parses a given message. Executes the command if it is a command and locally executable. Else sends it
     * to the server for further processing.
     */
    virtual void ParseMessage(const FText& Message);



    /**
     * If the client receives a message from the server, but has just logged in. Their widgets might not yet have
     * been initialized by the time the message arrives.
     */
    TQueue<FPrivateMessagePreConstruct> PreChatWidgetConstructionQueue = TQueue<FPrivateMessagePreConstruct>();

private:

    UFUNCTION(Server, Reliable, WithValidation)
    void QueueMessage_ServerRPC(const FText& Message);

    UFUNCTION(Client, Reliable)
    void AddMessageToChatLog_ClientRPC(const FString& Sender, const FText& Message);
};
