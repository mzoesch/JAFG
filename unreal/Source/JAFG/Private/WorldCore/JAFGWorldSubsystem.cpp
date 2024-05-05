// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/JAFGWorldSubsystems.h"

#include "WorldCore/RegisteredWorldNames.h"

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

    if (Outer->GetWorld()->GetName() == RegisteredWorlds::World)
    {
        return true;
    }

    return false;
}

// --------------------------------------------------------------------------

UJAFGTickableWorldSubsystem::UJAFGTickableWorldSubsystem(void) : Super()
{
    return;
}

bool UJAFGTickableWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    if (Outer->GetWorld()->GetName() == RegisteredWorlds::World)
    {
        return true;
    }

    return false;
}
