// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChatMessage.generated.h"

UENUM(NotBlueprintable, NotBlueprintType)
namespace EChatMessageType
{

enum Type : uint8
{
    Player,
    Client,
    Authority,
    Custom,
};

}

USTRUCT(NotBlueprintable, NotBlueprintType)
struct CHAT_API FChatMessage final
{
    GENERATED_BODY()

    FChatMessage() = default;
    explicit FChatMessage(const EChatMessageType::Type Type, const FString& Sender, const FText& Message)
    : MessageType(Type), Sender(Sender), Message(Message)
    {
    }
    explicit FChatMessage(const FString& Sender, const FText& Message)
    : FChatMessage(EChatMessageType::Player, Sender, Message)
    {
    }

    UPROPERTY( /* Replicated */ )
    TEnumAsByte<EChatMessageType::Type> MessageType;

    UPROPERTY( /* Replicated */ )
    FString                Sender;

    UPROPERTY( /* Replicated */ )
    FText                  Message;
};
