// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ChatComponent.generated.h"

class UChatMenu;
class UShippedWorldChatCommandRegistry;
struct FChatMessage;

UCLASS(Abstract, NotBlueprintable)
class JAFGEXTERNALCORE_API UChatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    FORCEINLINE virtual auto GetShippedWorldChatCommands(void) const -> UShippedWorldChatCommandRegistry*
        PURE_VIRTUAL(UChatComponent::GetShippedWorldChatCommands, return nullptr;)

    FORCEINLINE virtual auto AddMessageToChatLog(const FChatMessage& Message) -> void
        PURE_VIRTUAL(UChatComponent::AddMessageToChatLog)
    FORCEINLINE virtual auto ClearAllMessagesFromChatLog(void) -> void
        PURE_VIRTUAL(UChatComponent::ClearAllMessagesFromChatLog)

    FORCEINLINE virtual auto QueueCommandForServer(const FText& StdIn) -> void
        PURE_VIRTUAL(UChatComponent::QueueCommandForServer)
};
