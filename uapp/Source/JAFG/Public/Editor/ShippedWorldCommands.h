// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "ShippedWorldCommands.generated.h"

JAFG_VOID

#define CV_CHUNK_PREVIEW TEXT("j.ShowChunkPreview")
#define CV_CHUNK_BORDERS TEXT("j.ShowChunkBorders")

/**
 * Console commands that are being shipped.
 */
UCLASS(NotBlueprintable)
class JAFG_API UShippedWorldCommands : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize(void) override;
    // ~UWorldSubsystem implementation

    static auto Get(const UObject& Context) -> UShippedWorldCommands*;
    static auto Get(const UObject* Context) -> UShippedWorldCommands*;

    auto ShowChunkPreview(void) const -> bool;
    auto ShowChunkBorders(void) const -> bool;

    /** For JAFG variables only. */
    auto AttachToConsoleVariable(const FString& ConsoleVariableName, const FConsoleVariableDelegate& Delegate) -> FDelegateHandle;
    auto DetachFromConsoleVariable(const FString& ConsoleVariableName, const FDelegateHandle& Handle) -> bool;

private:

    void RegisterConsoleCommands(void);
    void UnregisterConsoleCommands(void);
};
