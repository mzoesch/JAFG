// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/LocalPlayerSubsystem.h"

#include "CustomInputSubsystem.generated.h"

JAFG_VOID

/**
 * The subsystem to actually add all the input bindings to JAFG Input Subsystem.
 */
UCLASS(NotBlueprintable)
class JAFG_API UCustomInputSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

private:

    void AddAllKeyMappings(void) const;
};
