// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/JAFGWorldSubsystems.h"

#include "RegisteredWorldNames.h"

bool WorldStatics::IsInGameWorld(const UObject* Outer)
{
    return
        Outer->GetWorld()->GetName() == RegisteredWorlds::World
     || Outer->GetWorld()->GetName() == RegisteredWorlds::Dev;
}

bool WorldStatics::IsInGameWorldExcludingDev(const UObject* Outer)
{
    return WorldStatics::IsInGameWorld(Outer) && Outer->GetWorld()->GetName() != RegisteredWorlds::Dev;
}

bool WorldStatics::IsInDevWorld(const UObject* Outer)
{
    return Outer->GetWorld()->GetName() == RegisteredWorlds::Dev;
}

UJAFGWorldSubsystem::UJAFGWorldSubsystem(void) : Super()
{
    return;
}

bool UJAFGWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return WorldStatics::IsInGameWorld(Outer);
}

UJAFGWorldSubsystemNoDev::UJAFGWorldSubsystemNoDev(void) : Super()
{
    return;
}

bool UJAFGWorldSubsystemNoDev::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return WorldStatics::IsInDevWorld(Outer) == false;
}

// --------------------------------------------------------------------------

UJAFGTickableWorldSubsystem::UJAFGTickableWorldSubsystem(void) : Super()
{
    this->TickInterval      = 0.0f;
    this->TimeSinceLastTick = 0.0f;

    return;
}

void UJAFGTickableWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->TickInterval      = this->DefaultTickInterval;

    /*
     * We want to call the tick method in the first tick of this subsystem begin play.
     * Minus 100 seconds, else we would get an immediate float overflow in the first tick of the subsystem.
     */
    this->TimeSinceLastTick = FLT_MAX - 100.0f;

    return;
}

bool UJAFGTickableWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return WorldStatics::IsInGameWorld(Outer);
}

void UJAFGTickableWorldSubsystem::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->TickInterval == 0.0f)
    {
        this->MyTick(DeltaTime);
        return;
    }

    this->TimeSinceLastTick += DeltaTime;

    if (this->TimeSinceLastTick >= this->TickInterval)
    {
        this->MyTick(DeltaTime);
        this->TimeSinceLastTick -= this->TickInterval;

        /* May happen during lag spikes. We do not want to tick than every frame */
        if (this->TimeSinceLastTick > this->TickInterval)
        {
            this->TimeSinceLastTick = 0.0f;
        }
    }

    return;
}

UJAFGTickableWorldSubsystemNoDev::UJAFGTickableWorldSubsystemNoDev(void) : Super()
{
    return;
}

bool UJAFGTickableWorldSubsystemNoDev::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return WorldStatics::IsInDevWorld(Outer) == false;
}
