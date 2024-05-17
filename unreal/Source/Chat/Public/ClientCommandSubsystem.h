// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "CommonChatStatics.h"

#include "ClientCommandSubsystem.generated.h"

typedef FString FClientCommand;

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

    bool IsRegisteredClientCommand(const FText& StdIn);
    bool IsRegisteredClientCommand(const FClientCommand& Command) const;

    void ExecuteCommand(const FText& StdIn, int32& OutErrorCode, FString& OutResponse) const;
    void ExecuteCommand(const FClientCommand& Command, const TArray<FString>& Args, int32& OutErrorCode, FString& OutResponse) const;
};
