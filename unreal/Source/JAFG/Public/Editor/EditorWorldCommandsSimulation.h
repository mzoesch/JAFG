// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"

#include "EditorWorldCommandsSimulation.generated.h"

JAFG_VOID

/**
 * Console commands that are being registered when the editor is running in simulation mode.
 * @note These commands will never be available outside the editor as only the editor supports game simulation.
 */
UCLASS(NotBlueprintable)
class JAFG_API UEditorWorldCommandsSimulation : public UJAFGWorldSubsystem
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

    IConsoleCommand* ToggleDebugScreenCommand;

    void ToggleDebugScreen(void) const;

    void RegisterConsoleCommands(void);
    void UnregisterConsoleCommands(void);
};
