// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "EditorWorldCommandsDedSv.generated.h"

JAFG_VOID

/**
 * Console commands that are being registered when a dedicated server is running.
 */
UCLASS(NotBlueprintable)
class JAFG_API UEditorWorldCommandsDedSv : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    // UWorldSubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize(void) override;
    // ~UWorldSubsystem implementation

private:

    IConsoleCommand* ShowVerticalChunksOnDedicatedServerCommand;
    IConsoleCommand* ShowPersistentVerticalChunksOnDedicatedServerCommand;

    void ShowVerticalChunksOnDedicatedServer(void) const;
    void ShowPersistentVerticalChunksOnDedicatedServer(void) const;

    void RegisterConsoleCommands(void);
    void UnregisterConsoleCommands(void);
};
