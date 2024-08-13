// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "JAFGWorldSubsystems.generated.h"

namespace WorldStatics
{

JAFGEXTERNALCORE_API auto IsInGameWorld(const UObject* Outer) -> bool;
JAFGEXTERNALCORE_API auto IsInGameWorldExcludingDev(const UObject* Outer) -> bool;
JAFGEXTERNALCORE_API auto IsInDevWorld(const UObject* Outer) -> bool;

}

/**
 * A subsystem that only exists in a real game world.
 * Meaning a world where we generate chunks, the user can interact with the world, etc.
 *
 * Not in the menu, transition, or loading screen world.
 * Note that this maps *includes* the development world. Use UJAFGWorldSubsystemNoDev if this subsystem should
 * not exist in the development world.
 */
UCLASS(Abstract)
class JAFGEXTERNALCORE_API UJAFGWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGWorldSubsystem();

    // UWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~UWorldSubsystem implementation
};

UCLASS(Abstract)
class JAFGEXTERNALCORE_API UJAFGWorldSubsystemNoDev : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGWorldSubsystemNoDev();

    // UWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~UWorldSubsystem implementation
};

/**
 * A subsystem that only exists in a real game world.
 * Meaning a world where we generate chunks, the user can interact with the world, etc.
 *
 * Not in the menu, transition, or loading screen world.
 * Note that this maps *includes* the development world. Use UJAFGTickableWorldSubsystemNoDev if this subsystem should
 * not exist in the development world.
 */
UCLASS(Abstract)
class JAFGEXTERNALCORE_API UJAFGTickableWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGTickableWorldSubsystem();

    // UWorldSubsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    // ~UWorldSubsystem implementation

    // UWorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto Tick(const float DeltaTime) -> void final override ;
    // ~UWorldSubsystem implementation

    /**
     * Override this method to get access to the tick event.
     * This tick is called every UJAFGTickableWorldSubsystem#TickInterval seconds.
     */
    virtual auto MyTick(const float DeltaTime) -> void { }

    /**
     * @param Interval The interval in seconds after which the Tick function should be called.
     */
    auto SetTickInterval(const float Interval) -> void
    {
        this->TickInterval = Interval;
    }

private:

    const float DefaultTickInterval = 0.0f;
    /** After how many seconds, the Tick function should be called. A value of zero means every tick. */
    float TickInterval;
    float TimeSinceLastTick;
};

UCLASS(Abstract)
class JAFGEXTERNALCORE_API UJAFGTickableWorldSubsystemNoDev : public UJAFGTickableWorldSubsystem
{
    GENERATED_BODY()

public:

    UJAFGTickableWorldSubsystemNoDev();

    // UWorldSubsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    // ~UWorldSubsystem implementation
};
