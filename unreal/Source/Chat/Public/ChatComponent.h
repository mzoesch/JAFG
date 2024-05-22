// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ChatComponent.generated.h"

class UChatMenu;
class AWorldPlayerController;
class UClientCommandSubsystem;
class UServerCommandSubsystem;

struct FPrivateMessagePreConstruct final
{
    FString Sender;
    FText   Message;
};

UCLASS(NotBlueprintable)
class CHAT_API UChatComponent : public UActorComponent
{
    GENERATED_BODY()

    friend UClientCommandSubsystem;
    friend UServerCommandSubsystem;

public:

    explicit UChatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    /** Because this component should always be attached to a subclass of this controller. */
    auto GetPredictedOwner(void) const -> AWorldPlayerController*;
    /**
     * Only on the owing instance of the game as it is a widget.
     * @retun The owning chat menu if valid else nullptr.
     */
    auto GetChatMenu(void) const -> UChatMenu*;
    auto GetSafeChatMenu(void) const -> UChatMenu*;

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

    static constexpr int32 MaxChatStdInLogBuffered = 0xFF;
    /** Sorted chronology from most recent to least. */
    TArray<FText> ChatStdInLog;

private:

    FString GetPlayerDisplayName(void) const;

    UFUNCTION(Server, Reliable, WithValidation)
    void QueueMessage_ServerRPC(const FText& Message);

    void BroadcastMessage(const FText& Message, const bool bAsAuthority = false) const;
    UFUNCTION(Client, Reliable)
    void AddMessageToChatLog_ClientRPC(const FString& Sender, const FText& Message);
    void AddMessageToChatLog(const FString& Sender, const FText& Message);
};
