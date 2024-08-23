// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChatMessage.h"
#include "Components/ActorComponent.h"

#include "ChatComponent.generated.h"

class UChatMenu;
class AWorldPlayerController;
class UClientCommandSubsystem;
class UServerCommandSubsystem;

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
    TQueue<FChatMessage> PreChatWidgetConstructionQueue = TQueue<FChatMessage>();

    static constexpr int32 MaxChatStdInLogBuffered = 0xFF;
    /** Sorted chronology from most recent to least. */
    TArray<FText> ChatStdInLog;

private:

    FString GetPlayerDisplayName(void) const;

    UFUNCTION(Server, Reliable, WithValidation)
    void QueueMessage_ServerRPC(const FChatMessage& Message);

    void BroadcastMessage(const FChatMessage& Message) const;
    UFUNCTION(Client, Reliable)
    void AddMessageToChatLog_ClientRPC(const FChatMessage& Message);
    void AddMessageToChatLog(const FChatMessage& Message);
};
