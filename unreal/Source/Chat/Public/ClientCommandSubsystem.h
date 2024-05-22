// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "CommonChatStatics.h"

#include "ClientCommandSubsystem.generated.h"

typedef FString FClientCommand;
typedef TFunction<
    void(
        TArray<FString> InArgs,
        CommandReturnCode& OutErrorCode,
        FString& OutResponse
    )
> FClientCommandCallback;

UCLASS(NotBlueprintable)
class CHAT_API UClientCommandSubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Deinitialize(void) override;
    // ~UWorldSubsystem implementation

    auto IsRegisteredClientCommand(const FText& StdIn) const -> bool;
    auto IsRegisteredClientCommand(const FClientCommand& Command) const -> bool;
    void ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const;
    void ExecuteCommand(const FClientCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const;

private:

    TMap<FClientCommand, FClientCommandCallback> ClientCommands;

    void InitializeAllCommands(void);

    auto OnHelpCommand(const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const -> void;

};
