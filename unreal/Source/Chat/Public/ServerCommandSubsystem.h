// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "CommonChatStatics.h"

#include "ServerCommandSubsystem.generated.h"

class UChatComponent;

typedef FString FServerCommand;
typedef TFunction<
    void(
        const UChatComponent* Owner,
        TArray<FString> InArgs,
        CommandReturnCode& OutErrorCode,
        FString& OutResponse
    )
> FServerCommandCallback;

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
    auto ExecuteCommand(const UChatComponent* Owner, const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;
    auto ExecuteCommand(const UChatComponent* Owner, const FServerCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;

private:

    TMap<FServerCommand, FServerCommandCallback> ServerCommands;

    void InitializeAllCommands(void);

    auto OnBroadcastCommand(const UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;
    auto OnFlyCommand(const UChatComponent* Owner, const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;
};
