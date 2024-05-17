// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "CommonChatStatics.h"

#include "ServerCommandSubsystem.generated.h"

typedef FString FServerCommand;
typedef TFunction<void(TArray<FString> InArgs, CommandReturnCode& OutErrorCode, FString& OutResponse)> FServerCommandCallback;

UCLASS(NotBlueprintable)
class CHAT_API UServerCommandSubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Deinitialize() override;
    // ~UWorldSubsystem implementation

    bool IsRegisteredCommand(const FText& StdIn) const;
    bool IsRegisteredCommand(const FServerCommand& Command) const;
    void ExecuteCommand(const FText& StdIn, CommandReturnCode& OutReturnCode, FString& OutResponse) const;
    void ExecuteCommand(const FServerCommand& Command, const TArray<FString>& Args, CommandReturnCode& OutReturnCode, FString& OutResponse) const;

private:

    TMap<FServerCommand, FServerCommandCallback> ServerCommands;

    void InitializeAllCommands(void);

    void OnBroadcastCommand(const TArray<FString>& InArgs, CommandReturnCode& OutReturnCode, FString& OutResponse) const;
};
