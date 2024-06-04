// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "CommonChatStatics.h"

#include "ServerCommandSubsystem.generated.h"

#define SERVER_COMMAND_SIG                                                                                       \
    UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse

class UChatComponent;

typedef FString                             FServerCommand;
typedef TFunction<void(SERVER_COMMAND_SIG)> FServerCommandCallback;

UCLASS(NotBlueprintable)
class CHAT_API UServerCommandSubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    // UWorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto Deinitialize(void) -> void override;
    // ~UWorldSubsystem implementation

    auto IsRegisteredCommand(const FText& StdIn) const -> bool;
    auto IsRegisteredCommand(const FServerCommand& Command) const -> bool;
    auto ExecuteCommand(UChatComponent* Owner, const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;
    auto ExecuteCommand(UChatComponent* Owner, const FServerCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;

protected:

    virtual void InitializeAllCommands(void);

    UChatComponent* GetTargetBasedOnArgs(const TArray<FString>& Args, CommandReturnCode& OutReturnCode, const int32 Index = 0) const;

private:

    TMap<FServerCommand, FServerCommandCallback> ServerCommands;

    auto OnHelpCommand(SERVER_COMMAND_SIG) const -> void;

    auto OnBroadcastCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnFlyCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnAllowInputFlyCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnShowOnlinePlayersCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnKickCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnChangeDisplayNameCommand(SERVER_COMMAND_SIG) const -> void;
    auto OnGiveAccumulatedCommand(SERVER_COMMAND_SIG) const -> void;
};
