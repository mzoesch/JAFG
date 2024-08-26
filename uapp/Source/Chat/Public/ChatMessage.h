// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSettingsLocal.h"

#include "ChatMessage.generated.h"

class UJAFGSettingsLocal;

UENUM(NotBlueprintable, NotBlueprintType)
namespace EChatMessageType
{

enum Type : uint8
{
    /** Remote connection reports a message. Replicated by broadcast. */
    Player,

    /** Owning client reports a message. Not replicated. */
    Client,

    /**
     * The authority reports a message. Conditionally replicated to selected remotes by broadcast.
     * Beware that client and authority might be the same instance.
     * We decide the type of message based on the generation of this message. Meaning if this message may be also
     * produced by an instance of the game that acts as a standalone client, then a message is considered to be from
     * precisely this owning client even if it was produced by the authority.
     */
    Authority,
};

}

UENUM(NotBlueprintable, NotBlueprintType)
namespace EChatMessageFormat
{

enum Type : uint8
{
    /** Default with the context of EChatMessageType. */
    Default,

    /*----------------------------------------------------------------------------
        Allowed by EChatMessageType::Player
    ----------------------------------------------------------------------------*/

    Whisper,

    /*----------------------------------------------------------------------------
        Allowed by EChatMessageType::Client and EChatMessageType::Authority
    ----------------------------------------------------------------------------*/

    Verbose,
    Info,
    Warning,
    Error,
    Success,

    /*----------------------------------------------------------------------------
        Custom formatting inside of the message.
    ----------------------------------------------------------------------------*/

    Custom,
};

}

auto LexToColor(UJAFGSettingsLocal* Settings, const EChatMessageType::Type Type, const EChatMessageFormat::Type Format) -> FColor;
auto LexToSlateColor(UJAFGSettingsLocal* Settings, const EChatMessageType::Type Type, const EChatMessageFormat::Type Format) -> FSlateColor;

USTRUCT(NotBlueprintable, NotBlueprintType)
struct CHAT_API FChatMessage final
{
    GENERATED_BODY()

    //////////////////////////////////////////////////////////////////////////
    // Constructors
    FChatMessage() = default;
    explicit FChatMessage(const EChatMessageType::Type Type, const EChatMessageFormat::Type Format, const FString& Sender, const FText& Message)
    : Type(Type), Format(Format), Sender(Sender), Message(Message)
    {
    }
    explicit FChatMessage(const EChatMessageType::Type Type, const EChatMessageFormat::Type Format, const FString& Sender, const FString& Message)
    : FChatMessage(Type, Format, Sender, FText::FromString(Message))
    {
    }
    explicit FChatMessage(const EChatMessageType::Type Type, const EChatMessageFormat::Type Format, const FText& Message)
    : FChatMessage(Type, Format, TEXT(""), Message)
    {
    }
    explicit FChatMessage(const EChatMessageType::Type Type, const EChatMessageFormat::Type Format, const FString& Message)
    : FChatMessage(Type, Format, FText::FromString(Message))
    {
    }
    explicit FChatMessage(const EChatMessageType::Type Type, const FString& Sender, const FText& Message)
    : FChatMessage(Type, EChatMessageFormat::Default, Sender, Message)
    {
    }
    explicit FChatMessage(const EChatMessageType::Type Type, const FString& Sender, const FString& Message)
    : FChatMessage(Type, Sender, FText::FromString(Message))
    {
    }
    explicit FChatMessage(const FString& Sender, const FText& Message)
    : FChatMessage(EChatMessageType::Player, Sender, Message)
    {
    }
    explicit FChatMessage(const FString& Sender, const FString& Message)
    : FChatMessage(Sender, FText::FromString(Message))
    {
    }
    explicit FChatMessage(const FText& Message)
    : FChatMessage(EChatMessageType::Client, TEXT(""), Message)
    {
    }
    explicit FChatMessage(const FString& Message)
    : FChatMessage(FText::FromString(Message))
    {
    }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY( /* Replicated */ )
    TEnumAsByte<EChatMessageType::Type> Type;

    UPROPERTY( /* Replicated */ )
    TEnumAsByte<EChatMessageFormat::Type> Format;

    UPROPERTY( /* Replicated */ )
    FString Sender;

    UPROPERTY( /* Replicated */ )
    FText Message;
};
