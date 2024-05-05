// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/WorldSubsystem.h"

#include "JAFGWorldSubsystems.generated.h"

JAFG_VOID

/**
 * A subsystem that only exists in a real game world.
 * Meaning a world where we generate chunks, the user can interact with the world, etc.
 *
 * Not in the menu, transition, or loading screen world, or other development worlds.
 */
UCLASS(Abstract)
class JAFG_API UJAFGWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGWorldSubsystem();

    // UWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~UWorldSubsystem implementation
};

/**
 * A subsystem that only exists in a real game world.
 * Meaning a world where we generate chunks, the user can interact with the world, etc.
 *
 * Not in the menu, transition, or loading screen world, or other development worlds.
 */
UCLASS(Abstract)
class JAFG_API UJAFGTickableWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGTickableWorldSubsystem();

    // UWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~UWorldSubsystem implementation
};
