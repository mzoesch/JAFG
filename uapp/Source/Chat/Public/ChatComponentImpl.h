// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChatMessage.h"
#include "Chat/ChatComponent.h"

#include "ChatComponentImpl.generated.h"

class UChatMenu;
class AWorldPlayerController;

UCLASS(NotBlueprintable)
class CHAT_API UChatComponentImpl final : public UChatComponent
{
    GENERATED_BODY()

public:

    explicit UChatComponentImpl(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UActorComponent implementation
    virtual void BeginPlay(void) override;
    // ~UActorComponent implementation

public:

    // UChatComponent implementation
    virtual auto GetShippedWorldChatCommands(void) const -> UShippedWorldChatCommandRegistry* override;
    virtual auto AddMessageToChatLog(const FChatMessage& Message) -> void override;
    virtual auto ClearAllMessagesFromChatLog(void) -> void override;
    virtual auto QueueCommandForServer(const FText& StdIn) -> void override;
    // ~UChatComponent implementation

    /** Because this component should always be attached to a subclass of this controller. */
    auto GetPredictedOwner(void) const -> AWorldPlayerController*;
    auto GetPlayerDisplayName(void) const -> FString;

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
    auto ParseMessage(const FText& Message) -> void;

    /**
     * If the client receives a message from the server, but has just logged in. Their widgets might not yet have
     * been initialized by the time the message arrives.
     */
    TQueue<FChatMessage> PreChatWidgetConstructionQueue = TQueue<FChatMessage>();

    static constexpr int32 MaxChatStdInLogBuffered = 0xFF;
    /** Sorted chronology from most recent to least. */
    TArray<FText> ChatStdInLog;

private:

    UFUNCTION(Server, Reliable, WithValidation)
    void QueueMessage_ServerRPC(const FText& Message);
    UFUNCTION(Server, Reliable, WithValidation)
    void QueueCommand_ServerRPC(const FText& Command);

    void BroadcastMessage(const FChatMessage& Message) const;
    UFUNCTION(Client, Reliable)
    void AddMessageToChatLog_ClientRPC(const FChatMessage& Message);
};
